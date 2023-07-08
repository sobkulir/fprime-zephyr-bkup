import os
import argparse
import typing

# -----------------
# These functions are hardcoded and should be changed if memory mapping changes(!)
def IS_ADDR_IN_RAM(addr: int) -> bool:
    return 0x24000000 <= addr < 0x24000000 + 320 * 1024

def IS_ADDR_IN_FLASH(addr: int) -> bool:
    return 0x08000000 <= addr < 0x8000000 + 1024 * 1024
# -----------------

def parse_args():
    '''Parse command line arguments'''
    parser = argparse.ArgumentParser(
        description='Analyze ELF files for Flash and RAM usage')
    parser.add_argument('elf_file_new', type=str, help='Path to the first ELF file')
    parser.add_argument("elf_file_old", type=str, nargs='?', default=None, help="Path to the second ELF file (optional)")

    return parser.parse_args()

# Expects an enumerable of dicts.
def tabulate(data):
    data = list(data)
    headers = data[0].keys() if data else []

    table = []
    table.append(headers)

    for item in data:
        row = [str(item.get(header, '')) for header in headers]
        table.append(row)

    max_lengths = [max(map(len, column)) for column in zip(*table)]
    formatted_table = []
    for row in table:
        formatted_row = [f"{value:{max_length}}" for value, max_length in zip(row, max_lengths)]
        formatted_table.append(formatted_row)

    out = ''
    for row in formatted_table:
        out += ' | '.join(row) + '\n'
    return out

class Symbol:
    def __init__(self, addr: int, size: int, sym_type: str, name: str):
        self.addr = addr
        self.size = size
        # flashTypes = ['T', 't', 'W', 'w', 'r', 'R']
        # ramTypes = ['d', 'D', 'b', 'B']
        self.type = sym_type
        self.name = name

class Symbols:

    def __init__(self, arr):
        self.arr = arr
        self.nameIndex = None
    
    def hasSymbolNamed(self, symbolName: str) -> Symbol | None:
        if self.nameIndex is None:
            self.nameIndex = {}
            for sym in self.arr:
                self.nameIndex[sym.name] = sym
        
        return self.nameIndex.get(symbolName, None)

    def __str__(self):
        return Symbols.symbolArrStr(self.arr)

    @staticmethod
    def symbolArrStr(symbol_arr):
        symbol_map = map(lambda x: {"Name": x.name, "Addr": hex(x.addr), "Size": x.size, "Type": x.type}, symbol_arr)
        return tabulate(symbol_map)

    @staticmethod
    def fromElfFile(elf_file) -> 'Symbols':
        elf_dir = os.path.dirname(elf_file)
        symbols_file = os.path.join(elf_dir, 'symbols.txt')
        if 0 != os.system(f"nm -CS --size-sort {elf_file} > {symbols_file}"):
            raise Exception("Failed to run nm command")

        symbols = []

        with open(symbols_file, "r") as file:
            for line in file:
                line = line.strip()
                sym = Symbol(
                    addr=int(line[:8], 16),
                    size=int(line[9:17], 16),
                    sym_type=line[18].strip(),
                    name=line[20:].strip()
                )
                symbols.append(sym)

        return Symbols(symbols)

    @staticmethod
    def deduplicated(symbols: 'Symbols') -> 'Symbols':
        """
        Deduplicates symbols. Two symbols are considered duplicates if they have the
        same address and size. If symbols are duplicates, the one appearing first is kept.
        """

        sortedArr = sorted(symbols.arr, key=lambda x: x.addr)
        deduplicatedArr = []
    
        for i, cur_sym in enumerate(sortedArr):
            if i == len(sortedArr) - 1:
                deduplicatedArr.append(sortedArr[i])
                break

            next_sym = sortedArr[i + 1]

            # Skip duplicate or overruning symbols.
            if cur_sym.addr == next_sym.addr and cur_sym.size == next_sym.size:
                continue
            elif cur_sym.addr + cur_sym.size > next_sym.addr:
                print("Warning: Overrun of ", cur_sym, " and ", next_sym)
                continue
            else:
                deduplicatedArr.append(cur_sym)

        return Symbols(deduplicatedArr)

class Section:
    def __init__(self, name: str, sec_type: str, addr: int, size: int):
        self.name = name
        self.type = sec_type
        self.addr = addr
        self.size = size
        self.foundSize = 0

class Sections:
    def __init__(self, arr):
        self.arr = arr
        self.nameIndex = None

    @staticmethod
    def fromElfFile(elf_file) -> 'Sections':
        elf_dir = os.path.dirname(elf_file)
        sections_file = os.path.join(elf_dir, 'sections.txt')

        if 0 != os.system(f"readelf -CS {elf_file} > {sections_file}"):
            raise Exception("Failed to run readelf command")

        section_arr = []
        
        with open(sections_file, "r") as file:
            # Skip header and footer
            lines = file.readlines()[4:-5]
            for line in lines:
                sec = Section(
                    name=line[7:25].strip(),
                    sec_type=line[25:41].strip(),
                    addr=int(line[41:49].strip(), 16),
                    size=int(line[57:63].strip(), 16))
                section_arr.append(sec)
        
        return Sections(section_arr)

    def totalFlashUsed(self) -> int:
        return sum(list(map(lambda x: x.size, filter(lambda x: x.type == 'PROGBITS' and x.addr != 0, self.arr))))

    def totalRamUsed(self) -> int:
        return sum(list(map(lambda x: x.size, filter(lambda x: IS_ADDR_IN_RAM(x.addr), self.arr))))

    # Increases foundSize of the section where the symbol is found.
    # Returns number of sections where the symbol was found (I believe
    # the sections must be non-overlapping, but just in case).
    def addSymbol(self, sym: Symbol) -> int:
        count = 0
        for sec in self.arr:
            if sym.addr >= sec.addr and sym.addr < sec.addr + sec.size:
                sec.foundSize += sym.size
                count += 1
        
        return count

    def getNameIndex(self) -> typing.Dict[str, 'Section']:
        if self.nameIndex is not None:
            return self.nameIndex
        
        self.nameIndex = {}
        for sec in self.arr:
            self.nameIndex[sec.name] = sec
        
        return self.nameIndex

def single_elf_analysis(elf_file):
    symbols = Symbols.deduplicated(Symbols.fromElfFile(elf_file))
    sections = Sections.fromElfFile(elf_file)

    flashFound = 0
    ramFound = 0
    flashTotal = sections.totalFlashUsed()
    ramTotal = sections.totalRamUsed()

    for sym in symbols.arr:
        if IS_ADDR_IN_RAM(sym.addr):
            ramFound += sym.size
        
        # If the symbol is of type 'd/D', it could be data copied into RAM, but first it needs
        # to be stored on flash so we need to account for it. 
        if IS_ADDR_IN_FLASH(sym.addr) or sym.type in ['d', 'D']:
            flashFound += sym.size

        sym_occurences = sections.addSymbol(sym)
        if sym_occurences != 1:
            print(f"Warning: Symbol found {sym_occurences} times: {sym}")

    print(f"Flash:\t{flashFound} / {flashTotal}")
    print(f"RAM:\t{ramFound} / {ramTotal}")
    print()

    sections_map = map(lambda x: {"Name": x.name, "Type": x.type, "Addr": hex(x.addr), "Size": x.size, "FoundSize": x.foundSize}, sections.arr)
    print(tabulate(sections_map))


def diff_elf_analysis_sections(elf_file_new, elf_file_old):
    sections_new = Sections.fromElfFile(elf_file_new)
    sections_old = Sections.fromElfFile(elf_file_old)

    new_flash = sections_new.totalFlashUsed()
    old_flash = sections_old.totalFlashUsed()
    new_ram = sections_new.totalRamUsed()
    old_ram = sections_old.totalRamUsed()
    print(f'Flash: {new_flash} ({old_flash}) [{new_flash - old_flash}]')
    print(f'Ram: {new_ram} ({old_ram}) [{new_ram - old_ram}]')

    print()

    removed = []
    same = []
    added = []
    for sec_old in sections_old.arr:
        sec_new = sections_new.getNameIndex().get(sec_old.name, None)
        if sec_new is None:
            removed.append(sec_old)
        else:
            same.append((sec_new, sec_old))

    for sec_new in sections_new.arr:
        if sec_new.name not in sections_old.getNameIndex():
            added.append(sec_new)

    removed_added_sec_mapping = lambda x, prefix: {"Name": f'{prefix}{x.name}', "Type": x.type, "Addr": x.addr, "Size": x.size}
    removed_map = map(lambda x: removed_added_sec_mapping(x, '(-) '), removed)
    added_map = map(lambda x: removed_added_sec_mapping(x, '(+) '), added)

    same_map = map(lambda new_old: {
        "Name": new_old[0].name,
        "Type": f'{new_old[0].type} ({new_old[1].type})',
        "Addr": f'{hex(new_old[0].addr)} ({hex(new_old[1].addr)})',
        "Size": f'{new_old[0].size} ({new_old[1].size})',
    }, same)
    print(tabulate(list(removed_map) + list(same_map) + list(added_map)))

def diff_elf_analysis_symbols(elf_file_new, elf_file_old):
    symbols_new = Symbols.deduplicated(Symbols.fromElfFile(elf_file_new))
    symbols_old = Symbols.deduplicated(Symbols.fromElfFile(elf_file_old))

    removed = []
    changed = []
    added = []

    for sym_old in symbols_old.arr:
        sym_new = symbols_new.hasSymbolNamed(sym_old.name)
        if sym_new is None:
            removed.append(sym_old)
        elif sym_new.size != sym_old.size or sym_new.type != sym_old.type:
            changed.append((sym_new, sym_old))

    for sym in symbols_new.arr:
        if symbols_old.hasSymbolNamed(sym.name) is None:
            added.append(sym)

    print('------------- Removed -------------')
    print(Symbols.symbolArrStr(removed))
    print()
    print('------------- Changed -------------')
    changed_map = map(lambda new_old: {
            "Name": new_old[0].name,
            "Addr": f'{hex(new_old[0].addr)} ({hex(new_old[1].addr)})',
            "Size": f'{new_old[0].size} ({new_old[1].size})',
            "Type": f'{new_old[0].type} ({new_old[1].type})'
        }, changed)
    print(tabulate(changed_map))
    print()
    print('-------------- Added --------------')
    print(Symbols.symbolArrStr(added))

if __name__ == '__main__':
    args = parse_args()
    if args.elf_file_old is None:
        single_elf_analysis(args.elf_file_new)
    else:
        diff_elf_analysis_sections(args.elf_file_new, args.elf_file_old)
        diff_elf_analysis_symbols(args.elf_file_new, args.elf_file_old)