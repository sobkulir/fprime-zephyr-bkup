#!/usr/bin/env python3

import argparse
import subprocess
from pathlib import Path

DEFAULT_IMAGE = 'rsobkuliak/fprime-zephyr:0.0.3'

# Parent directory of current working directory
DEFAULT_PROJECT_DIR = '../'


def parse_args():
    '''Parse command line arguments'''
    parser = argparse.ArgumentParser(
        description='run development environment inside a Docker container')
    parser.add_argument(
        '-f', '--force-devices', action='store_true',
        help='(dangerous) mounts `/dev` and runs the container with `--privileged` flag')
    parser.add_argument(
        '-p', '--project-dir', type=str, default=DEFAULT_PROJECT_DIR,
        help=f'zephyrproject directory (default: `{DEFAULT_PROJECT_DIR}`)')
    parser.add_argument(
        '-d', '--dry-run', action='store_true', help='show the commands without running it')

    parser.add_argument('image', type=str, nargs="?", default=DEFAULT_IMAGE,
                        help=f'docker image (default: `{DEFAULT_IMAGE}`')

    return parser.parse_args()


def get_device_fallthrough(is_force_devices):
    '''Allows us to flash the MCUs from the container.

    I got the ideas from here: https://stackoverflow.com/questions/24225647
    '''
    if is_force_devices:
        return ['--privileged', '-v', '/dev:/dev']
    else:
        # The device group 180 is for "USB block devices". If you run into troubles, try multiple groups:
        #   https://www.kernel.org/doc/Documentation/admin-guide/devices.txt
        return ['-v', '/dev/bus/usb:/dev/bus/usb', "--device-cgroup-rule=a 180:* rmw", '-v', '/dev/ttyUSB0:/dev/ttyUSB0']


def get_abs_path(path):
    '''Returns absolute path'''
    return Path(path).expanduser().absolute().resolve()


class Command:
    '''Stores command arguments'''

    def __init__(self, args, check=True):
        self.args = args
        self.check = check

    def __str__(self):
        cmd = ' '.join([str(x) for x in self.args])
        return cmd


def main():
    '''Run fprime-zephyr in Docker container'''
    args = parse_args()
    project_dir_abs = get_abs_path(args.project_dir)
    bash_history_abs = project_dir_abs.joinpath('.docker_bash_history')

    # To make Docker bind-mount the .bash_history as file and not directory,
    # it needs to exist on the host before.
    cmd_bash_history = Command(['touch', bash_history_abs])
    cmd_docker = Command(
        ['docker', 'run',
         '-it',
         '--rm',
         *get_device_fallthrough(args.force_devices),
         '-v', f'{project_dir_abs}:/zephyr-workspace',
         '-v', f'{bash_history_abs}:/home/user/.bash_history',
         '--hostname', 'fprime',
         '--network', 'host',
         args.image,
         ],
        check=False)

    for cmd in [cmd_bash_history, cmd_docker]:
        if args.dry_run:
            print(cmd)
        else:
            subprocess.run(cmd.args, check=cmd.check)


if __name__ == '__main__':
    main()
