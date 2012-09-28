# c

Simple shell command to work with files in console. Copy-paste style like in Finder or Explorer.

### Heaven for copy-pasters

I'm very happy with near everything in my favorite `zsh`. But I am worried every time when I need to copy a lot of different files from different parts of the disk.

Very convenient way to do it using the copy-paste paradigm, like in Explorer or Finder. Simply select the files, press `ctrl+c`, switch to the directory you require, press `ctrl+v`.

So, I am worried and once upon a time I said: "Enough". And so was `C` born.

### What is C?

`C` is a small Python3 script which trying to imitate `ctrl` or `cmd` key for files operation. How it works? It's simply like this:

```shell
$ cd /etc
$ c c passwd
+. /etc/passwd
$ c c sudoers sshd_config
+. /etc/sudoers
+. /etc/sshd_config
$ cd apache2
$ c c httpd.conf
+. /private/etc/apache2/httpd.conf
$ cd ~/backup
$ c v
Copying to `/home/bobuk/backup'
. /etc/sshd_config -> backup/sshd_config
. /etc/passwd -> backup/passwd
. /etc/apache2/httpd.conf -> backup/httpd.conf
. /etc/sudoers -> backup/sudoers
```

Here `c c` is a analogue for `ctrl+c` and `c v` for `ctrl+v`. Looks like a crap? **Yes!** But with nifty aliases it's can be a much easier. You can use your favorite `ctrl+commands` as a shell commands.

```shell
$ ls
README.md   TODO.todo   c       install.sh
$ cmd+c README.md install.sh
+. README.md
+. install.sh
$ mkdir release && cd $_
$ cmd+v
Copying to `/Users/bobuk/V/c/release'
. /Users/bobuk/V/c/README.md -> /Users/bobuk/V/c/release/README.md
. /Users/bobuk/V/c/install.sh -> /Users/bobuk/V/c/release/install.sh
$ ls -1
README.md
install.sh
$ cmd+c
+D /Users/bobuk/V/c/release
$ cd ..
$ cmd+del
Are you sure to remove 1 directories files (y/n)y
move to trash /Users/bobuk/V/c/release
Done
```

In this case I use `cmd+commands` because installation script recognize what it run on Darwin.

### Commands

```shell
$ cmd+h
List of commands:
    cmd+c mark files and directories for copying or moving
    cmd+v copy files and other stuff to current directory
    cmd+del remove selected files to trash
    cmd+p show a list of marked files
    cmd+R reset list of selection
    cmd+h show list of commands
```

### Features

This is a raw list of features. Must be reordered and completed.

* `cmd+c` command work as 'mark files' and take unlimited number of arguments.
* `cmd+c` with no arguments work as `cmd+c` with current directory argument.
* `-q` switch make all commands quiet.
* `-2` will stop commands from files list flushing. I.e. after `cmd+v` old files list still in buffer.
* `cmd+del` remove files into system trashcan if module `send2trash` is installed.

### Authors

Yes, it's me. All code and staff was writen by BoBuk.

### Licence

It's absolutely [CC0 public domain](http://creativecommons.org/publicdomain/zero/1.0/).
But copying with bad intents prohibited by real patsans law
