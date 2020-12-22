# notify

_Scriptable desktop notifications for Linux_

`notify` is a simple executable meant to do one thing and do it well - give 
users a way to notify end users when something important occurs on their 
system. This isn't a replacement for notifications that an application 
handles natively, but instead aims to be a better alternative to `notify-send`

## Goals

* [ ] Working across multiple distros 
* [ ] Working on multiple window managers (Gnome and KDE at least) 
* [ ] Working on headless systems  
* [ ] Offering means for user interaction with the notification (such as 
  buttons, signals, etc.)  
* [ ] Ability to specify a timeout (in minutes) that the notification expires 
  after
* [ ] Fully scriptable
* [ ] Attempt to not be locked in to a single platform

## What it looks like

Notifications are presented normally based on what your window manager provides.
Here are a couple of instances for what they look like when launched with the
following commands:

`./notify -t 2 -o First -v 1 -o Second -v 2 -o "Third Option" -v 3 "Notify <WM>
Example" "A simple example of a notification in <WM>"`

### Gnome

![Gnome notification](userdocs/assets/images/Gnome_notification_example.png)

### KDE

![KDE notification](userdocs/assets/images/KDE_notification_example.png)

## Usage

```
notify [options] <TITLE> <DESCRIPTION>

Options:
    --option|-o     An option, up to 3 are allowed. Options are displayed in
                    the notification buttons.
    --value|-v      A value, up to 3 are allowed. Values are printed when a
                    notification option is selected.
    --timeout|-t    Notification lifetime, in minutes.
    --icon|-i       The desired icon name to be displayed in the notification.
```

## Development Requirements

Below is a list of packages required for their various distributions to build
and test notify. If your distribution isn't listed, feel free to add it!

If you are interested in reading further information on development 
practices and how to contribute, see the 
[Development Userdocs](./userdocs/development/README.md).

### Fedora

dnf/yum

```
bash
clang
findutils
gmock-devel
gtest-devel
gtk3-devel
jq
lcov
libnotify-devel
llvm
make
python3-pip
```

pip3

```
compiledb
```

For quick setup, run `bin/configure` (requires sudo access, currently only for
Fedora systems).

### Ubuntu

apt-get

```
bash
build-essential
clang
clang-tools
findutils
gtk+-3-dev
jq
lcov
libgmock-dev
libgtest-dev
libnotify-dev
llvm
pkg-config
python3-pip
```

pip3

```
compiledb
```

### Other distros

**FIXME**: No instructions yet.

## Building

### Native

When you are building natively on your distro of choice, you will need to ensure
that you have the development requirements from above installed on your system.
Once that is all said and done, you should be able to build `notify` by simply
running `make` from your terminal.

It is worth noting that `make` on its own will run through everything in the
build process, including linting and testing, as well as outputting the
executable at the end. You may select individual build targets as well.

### Containerized

If you could like to develop and compile `notify` in a docker container, you are
more than welcome to do so. We have set up multiple build environemtns (or will,
given time) so you can build for your distribution of choice. See 
`containers/build/` for the currently-available list of container files.

**NOTE**: All container commands are meant to be run from the top-level 
directory of `notify`.

Building within a container file is relatively simple, and can be done like so:

#### Docker

**NOTE**: The docker instructions may be off. In theory, podman is API 
compatible with docker, but that may not always be the case, and the 
instructions were based off of podman.

The first thing you will need to do is build your build container:  
`docker build -f containers/build/Containerfile.fedora --tag=fedora_notify_build:latest`

Once you have your build container up and running, you can build `notify` like
so:  
`docker run -t --mount type=bind,source=.,destination=/notify --privileged --rm localhost/fedora_notify_build:latest`

#### Podman

The first thing you will need to do is build your build container:  
`podman build -f containers/build/Containerfile.fedora --tag=fedora_notify_build:latest`

Once you have your build container up and running, you can build `notify` like
so:  
`docker run -t --mount type=bind,source=.,destination=/notify --privileged --rm localhost/fedora_notify_build:latest`

# License

Apache 2
