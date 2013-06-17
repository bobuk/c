#!/usr/bin/env python3
# 2012 (c) Grigory Bakunov <thebobuk@ya.ru>
# Copying with bad intents prohibited by real patsans law

import sys
import os
import json
import itertools
import shutil
import platform

commands = ['c', 'v', 'del', 'p', 'R', 'h']
aliases  = {
    'help': 'h',
    'copy': 'c',
    'paste': 'p',
}


class C:
    N = ''
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'

    @classmethod
    def via_colorama(cls):
        cls.HEADER = '\033[35;40;1m'
        cls.OKBLUE = '\033[36;40;1m'
        cls.OKGREEN = '\033[32;40;1m'
        cls.WARNING = '\033[33;40;1m'
        cls.FAIL = '\033[31;40;1m'

    @classmethod
    def disable(cls):
        cls.HEADER = ''
        cls.OKBLUE = ''
        cls.OKGREEN = ''
        cls.WARNING = ''
        cls.FAIL = ''
        cls.ENDC = ''

    @classmethod
    def oskey(cls, cmd, color = True):
        if sys.platform == 'darwin':
            cmd = cmd.replace('{ctrl+', '{cmd+')
        return cmd.replace('{', C.OKGREEN if color else '`').replace('}', C.ENDC if color else "'")


class Storage:
    def __init__(self, path=os.path.expanduser("~/.ctrl-buff")):
        self.path = path
        self.clean()

    def clean(self):
        self.files = []
        self.dirs = []
        return self

    def load(self):
        try:
            with open(self.path, 'r') as fl:
                (self.files, self.dirs) = json.loads(fl.read())
        except IOError:
            pass
        return self

    def save(self):
        with open(self.path, 'w') as fl:
            fl.write(json.dumps((
                list(set(self.files)),
                list(set(self.dirs)),
            )))
        return self


class normal_cmd:
    "Dumb command"
    AUTOLOAD = False
    NEEDFILES = False
    AUTOSAVE = False
    AUTOCLEAN = False

    def __init__(self):
        self.args = []
        self.quiet = False
        self.canclean = True
        if self.AUTOLOAD:
            self.storage = Storage()
            self.storage.load()
            if self.NEEDFILES and not (self.storage.files or self.storage.dirs):
                raise Exception("Please select some files or dirs with {ctrl+c}")

    def output(self, mode, string):
        if not self.quiet or (mode == C.FAIL):
            sys.stdout.write(mode + string + C.ENDC + '\n')

    def with_args(self, args):
        if '-h' in args or '--help' in args:
            self.output(C.N, 'Help: ' + C.oskey(self.__doc__))
            sys.exit(-2)
        if '-q' in args or '--quiet' in args:
            self.quiet = True
            for x in ['-q', '--quiet']:
                if x in args:
                    args.remove(x)
        if '-2' in args or '--not-clean' in args:
            self.canclean = False
            for x in ['-2', '--not-clean']:
                if x in args:
                    args.remove(x)

        self.args += args

    def count(self):
        try:
            sf = (str(len(self.storage.files)) + ' files') if self.storage.files else ''
            sd = (str(len(self.storage.dirs)) + ' directories') if self.storage.dirs else ''
            if sf and sd:
                return sf + ' and ' + sd
            else:
                return sf if sf else sd
        except:
            return ''

    def clean(self):
        if self.canclean:
            self.storage.clean()
        else:
            self.output(C.WARNING, 'Buffer not wiped.')

    def doit(self):
        self.do()
        if self.AUTOCLEAN:
            self.clean()
        if self.AUTOSAVE:
            self.storage.save()

    def process(self):
        pass

    def do(self):
        raise Exception('Not Implemented')


class del_cmd(normal_cmd):
    """{ctrl+del} removes selected files to trash"""
    AUTOLOAD = True
    NEEDFILES = True
    AUTOSAVE = True
    AUTOCLEAN = True

    def do(self):
        if input("Are you sure to remove " + self.count() + ' files (y/n)') in ['y', 'Y', 'yes']:
            try:
                from send2trash import send2trash
            except:
                send2trash = shutil.rmtree
            for item in itertools.chain(self.storage.files, self.storage.dirs):
                self.output(C.OKGREEN, 'move to trash ' + item)
                send2trash(item)
            self.output(C.OKBLUE, 'Done')
        else:
            self.output(C.WARNING, '\tCanceled')


class R_cmd(normal_cmd):
    """{ctrl+R} resets list of selection"""
    # We can't do just AUTOCLEAN/AUTOSAVE because I want to ignore -2 argument
    def process(self):
        self.storage = Storage()

    def do(self):
        self.storage.save()


class h_cmd(normal_cmd):
    """{ctrl+h} shows list of commands"""

    def process(self):
        self.cmdlist = [eval(x + '_cmd').__doc__ for x in commands]

    def do(self):
        self.output(C.HEADER, 'List of commands:')
        for cmd in self.cmdlist:
            self.output(C.N, '\t' + C.oskey(cmd))


class p_cmd(normal_cmd):
    """{ctrl+p} shows a list of marked files"""
    AUTOLOAD = True

    def do(self):
        for x in self.storage.dirs:
            self.output(C.OKBLUE, 'D\t' + x)
        for x in self.storage.files:
            self.output(C.N, '.\t' + x)


class c_cmd(normal_cmd):
    """{ctrl+c} marks files and directories for copying or moving"""
    AUTOLOAD = True
    AUTOSAVE = True

    def process(self):
        self.files = []
        self.dirs = []
        if not self.args:
            self.args.append(os.getcwd())
        for x in self.args:
            p = os.path.realpath(x)
            if not os.path.exists(p):
                raise Exception('File ' + p + ' not found')
            if os.path.isfile(p):
                self.files.append(p)
            elif os.path.isdir(p):
                self.dirs.append(p)
            else:
                raise Exception('Unknown filetype "' + p + '"')

    def do(self):
        for x in self.dirs:
            self.output(C.OKBLUE, '+D ' + x)
        self.storage.dirs += self.dirs
        for x in self.files:
            self.output(C.N, '+. ' + x)
        self.storage.files += self.files


class vx_cmd_proto(normal_cmd):
    AUTOLOAD = True
    NEEDFILES = True
    AUTOSAVE = True
    AUTOCLEAN = True

    def vname(self, x, topath=os.getcwd()):
        name = os.path.basename(x)
        overwrite = False
        if os.path.exists(os.path.join(topath, name)):
            self.output(C.WARNING, 'File ' + name + ' exists.')
            try:
                newname = input('Old name `' + C.OKGREEN + name + C.ENDC + "', new name?\n>> ")
            except KeyboardInterrupt:
                newname = False
            if not newname:
                raise Exception('\nAbort.')
            if newname.startswith('*'):
                newname = name + newname[1:]
            elif newname.startswith('!'):
                newname = name
                overwrite = True
            if os.path.exists(os.path.join(topath, newname)) and not overwrite:
                raise Exception('Name `' + newname + "' exists.")
        else:
            newname = name
        newname = os.path.join(topath, newname)
        return (newname, overwrite)


class v_cmd(vx_cmd_proto):
    """{ctrl+v} copies files and other stuff to current directory"""

    def process(self):
        if not self.args:
            self.args.append(os.getcwd())

    def do(self):
        for curpath in self.args:
            self.output(C.HEADER, 'Copying to `' + curpath + "'")
            for x in self.storage.dirs:
                newname, overwrite = self.vname(x, curpath)
                if not overwrite:
                    self.output(C.OKBLUE, 'D ' + x + ' -> ' + newname)
                    shutil.copytree(x, newname, symlinks=True)
                else:
                    self.output(C.WARNING, 'D ' + x + ' -> ' + newname + ' overwrite')
                    suffix = '.bak.bak.bak'
                    shutil.copytree(x, newname + suffix, symlinks=True)
                    shutil.rmtree(newname)
            for x in self.storage.files:
                newname, overwrite = self.vname(x, curpath)
                if overwrite:
                    self.output(C.WARNING, '. ' + x + ' -> ' + newname + ' overwrite')
                    shutil.copy2(x, newname)
                else:
                    self.output(C.OKGREEN, '. ' + x + ' -> ' + newname)
                    shutil.copy2(x, newname)


def argparse_and_cmd(args=sys.argv[1:]):
    if len(args) < 1:
        raise Exception('First argument must be one of these: ' + ', '.join(commands))
    if args[0].startswith('-'):
        args[0] = args[0].lstrip('-')
    if args[0] in aliases:
        args[0] = aliases[args[0]]
    cmd = eval(args[0] + '_cmd()')
    cmd.with_args(args[1:])
    cmd.process()
    return cmd


def fetchname(name=sys.argv[0]):
    name = os.path.basename(name)
    if '+' in name:
        return name.rsplit('+')[-1]
    if '-' in name:
        return name.rsplit('-')[-1]
    return

if __name__ == '__main__':
    try:
        if platform.system().lower() == 'windows':
            try:
                from colorama import init
                init()
                C.via_colorama()
            except:
                C.disable()
                print('You probably would love to run command \'easy_install colorama\' at first ;)')
        name = fetchname()
        if name:
            cmd = argparse_and_cmd([name] + sys.argv[1:])
        else:
            cmd = argparse_and_cmd()
        if cmd:
            cmd.doit()
    except Exception as e:
        sys.stderr.write(C.FAIL + C.oskey(str(e), color=False) + C.ENDC + '\n')
        sys.exit(-1)
