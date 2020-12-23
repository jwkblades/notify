# Development

At the moment, the majority of the development information is located in the
[Contributing](./CONTRIBUTING.md) document, however the [FAQ](FAQ.md) may also
prove useful in answering some questions.

To simplify the steps needed to get involved, `notify` offers a number of
flavors of container images that provide a full-fledged build environment (as
well as being capable of running unit tests). There are a number of `oci-`
applications in the `bin/` directory which take care of all of the required
commands for setting up the build container, as well as running commands within
the container. Here is a little more information about what each of the `oci-`
executables do for you:
* `oci-build-container` allows you to build a container according to
  flavor, the available container files are in `containers/*/`. By default, this
  will build all available container files; however, it can by narrowed down by
  specifying `FLAVOR` and/ or `CONTAINER`
* `oci-make` runs make within a container of the desired flavor.
* `oci-exec` runs a command in the container runtime on your machine;
  currently `docker` and `podman` are supported.
* `oci-run` runs a command in a build container, once again
  based on `FLAVOR` and defaulting to `fedora`. You may also specify
  a `CONTAINER`.
* `oci-runtime` prints the determined container runtime for the system,
  currently supports `docker` and `podman`.
* `oci-pkg` packages notify up for a distribution based on `FLAVOR`, which
  defaults to `fedora`.

## Environment Variables

* `FLAVOR` what distro you are building for, such as `fedora`, or `ubuntu`.
* `CONTAINER` the container purpose that should be built, such as `build`, or
  `package`.

## Building a flavor

To create a build container of your desired flavor, you can run `FLAVOR=<FLAVOR>
./bin/oci-build-container`, alternatively you can just run `FLAVOR=<FLAVOR>
./bin/oci-make` to build the application once the build container is ready.

Building an application with a specified flavor will output its objects and
dependency files in a separate directory (for each flavor, or no sub-directory,
in the case of native). The executables will also automatically have the flavor
as a prefix.

## Running unit tests in a container

Running unit tests in the build container is fairly straight forward as well.
First you will need to build the `test` target. That can be done like so:

`FLAVOR=<FLAVOR> ./bin/oci-make test`

Once your flavor test executable has been compiled, you can run it within the
container via:

`FLAVOR=<FLAVOR> ./bin/oci-run ./<FLAVOR>-test`

## Packaging

Packaging has also been taken care of in a containerized fashion, though for the
time being the containers available for packing are relatively limited (Fedora
only).

To start a package build, run `oci-pkg` with optionally specifying the flavor of
package (per container flavor) to be built, like so:

`./bin/oci-pkg`  
`FLAVOR=fedora ./bin/oci-pkg`

The default `FLAVOR` is fedora.

## Code coverage

When determining code coverage, you should be able to do so by simply running
`./bin/coverage ./test`. It is currently best to do so on a native build, as it
will attempt to open the coverage report in your default browser (using
`xdg-open`), and will likely have difficulty doing so from within a build
container.
