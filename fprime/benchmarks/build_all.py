import os
import argparse

ALL_BENCHMARKS = ['1-zephyr', '1.1-zephyr-fpu', '1.2-zephyr-drivers']
SCRIPT_PATH = os.path.dirname(os.path.realpath(__file__))

def parse_args():
    '''Parse command line arguments'''
    parser = argparse.ArgumentParser(
        description='Build and analyze benchmarks. By default, all benchmarks are built and analyzed.')
    parser.add_argument('bench_name', type=str, nargs='?', default=None, help='Benchmark name')

    return parser.parse_args()

# https://stackoverflow.com/questions/287871/how-do-i-print-colored-text-to-the-terminal
class bcolors:
    ORANGE_BOLD = '\033[33m\033[1m'
    RED_BOLD = '\033[31m\033[1m'
    ENDC = '\033[0m'

def execute_cmd(desc, cmd, err):
    print(bcolors.ORANGE_BOLD + desc + bcolors.ENDC)
    print(cmd)
    if 0 != os.system(cmd):
        print(bcolors.RED_BOLD + f'Error: {err}' + bcolors.ENDC)
        exit(1)

def build_and_analyze(b_name):
    b_src = os.path.join(SCRIPT_PATH, b_name)
    b_build = os.path.join(SCRIPT_PATH, b_name, 'build')
    b_report = os.path.join(SCRIPT_PATH, 'report', b_name)

    execute_cmd(
        desc='[1/4] Building...',
        cmd=f'rm -rf {b_build} && west build -d {b_build} -b nucleo_h723zg -p=always {b_src}',
        err=f"Failed to build benchmark {b_name}")

    execute_cmd(
        desc=f'[2/4] Making report dir...',
        cmd=f'mkdir -p {b_report}',
        err=f'Failed to create report folder for {b_name}')

    execute_cmd(
        desc=f'[3/4] Copying ELF...',
        cmd=f'cp {b_build}/zephyr/zephyr.elf {b_report}',
        err=f'Failed to copy zephyr.elf for {b_name}')

    execute_cmd(
        desc=f'[4/4] Analyzing ELF...',
        cmd=f'python3 {SCRIPT_PATH}/analyze_elf.py {b_report}/zephyr.elf > {b_report}/results.txt',
        err=f'Failed to analyze ELF for {b_name}'
    )

if __name__ == '__main__':
    args = parse_args()
    
    benches_to_run = []
    if args.bench_name is not None:
        benches_to_run = [args.bench_name]
    else:
        benches_to_run = ALL_BENCHMARKS

    for b_name in benches_to_run:
        build_and_analyze(b_name)