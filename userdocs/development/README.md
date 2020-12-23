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
  flavor, the available container files are in `containers/build/`.
* `oci-make` runs make within a container of the desired flavor. If no
  `FLAVOR` environment variable is set, then `fedora` is used.
* `oci-exec` runs a command in the container runtime on your machine;
  currently `docker` and `podman` are supported.
* `oci-run` runs a command in a build container, once again
  based on `FLAVOR` and defaulting to `fedora`. You may also specify
  a `CONTAINER`, which is planned to be associated with the directory in
  `containers/*`, for instance `build` or `package`.
* `oci-runtime` prints the determined container runtime for the system,
  currently supports `docker` and `podman`.

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

To go about packaging, you can run `oci-run` again, this time with the
environment variable `CONTAINER=package` (to specify the container type,
normally that defaults to `build`), and then the packager script that you are
interested in. For instance:

`CONTAINER=package ./bin/oci-run ./pkg/fedora/packager.sh`

Once everything has completed with the packaging, the resulting packages should
be available in the build directory.

## Code coverage

When determining code coverage, you should be able to do so by simply running
`./bin/coverage ./test`. It is currently best to do so on a native build, as it
will attempt to open the coverage report in your default browser (using
`xdg-open`), and will likely have difficulty doing so from within a build
container.
