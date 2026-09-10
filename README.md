# ADSB Flight Tracker

A C++ project for interacting with the [OpenSky Network](https://opensky-network.org/) REST API. Currently supports fetching live aircraft state data, with historical flight-lookup support in progress.

## Features

- Fetches live aircraft state vectors from OpenSky's `/states/all` endpoint (anonymous access)
- Parses JSON responses using `nlohmann/json`
- Converts epoch timestamps into readable UTC date/time strings
- Built with CMake + vcpkg for cross-platform dependency management

## Requirements

- C++17 compiler (MSVC, GCC, or Clang)
- [CMake](https://cmake.org/) (3.15+)
- [vcpkg](https://github.com/microsoft/vcpkg) for dependency management
- Dependencies (installed via vcpkg):
  - [libcurl](https://curl.se/libcurl/) — HTTP requests
  - [nlohmann/json](https://github.com/nlohmann/json) — JSON parsing

## Setup

### 1. Install dependencies with vcpkg

```bash
vcpkg install curl:x64-windows
vcpkg install nlohmann-json:x64-windows
```

(Use the triplet matching your platform, e.g. `x64-linux`, `x64-osx`, if not on Windows.)

### 2. Configure CMake with the vcpkg toolchain

Point CMake at vcpkg's toolchain file, either via command line:

```bash
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=<path-to-vcpkg>/scripts/buildsystems/vcpkg.cmake
```

or via `.vscode/settings.json` if using VS Code + CMake Tools:

```json
{
  "cmake.configureSettings": {
    "CMAKE_TOOLCHAIN_FILE": "<path-to-vcpkg>/scripts/buildsystems/vcpkg.cmake"
  },
  "cmake.platform": "x64"
}
```

### 3. Build

```bash
cmake --build build
```

### 4. Run

```bash
./build/opensky        # Linux/macOS
.\build\Debug\opensky.exe   # Windows
```

## Authentication note

Anonymous access is sufficient for real-time endpoints like `/states/all`, which this project currently uses, but comes with **reduced rate limits**.

**Historical flight data endpoints** (`/flights/arrival`, `/flights/departure`, `/flights/aircraft`, etc.) require authentication. As of March 18, 2026, OpenSky no longer supports basic authentication with a username and password — authentication is done via the **OAuth2 client credentials flow**.

To use any historical/flight-lookup features:

1. Log in to your OpenSky account and go to the **Account** page.
2. Create a new API client to get a `client_id` and `client_secret`.
3. Use these credentials to request an access token from OpenSky's OAuth2 token endpoint.
4. Attach that token as a `Bearer` token in the `Authorization` header of your requests.

This project does not currently implement the OAuth2 flow — that's planned as a future addition. Without it, any request to the historical flight endpoints will fail with an authorization error rather than returning JSON data.

## Roadmap

- [ ] Implement OAuth2 client credentials flow for authenticated endpoints
- [ ] Add support for `/flights/arrival` and `/flights/departure`
- [ ] Add bounding-box filtering for `/states/all`
- [ ] Improve error handling / retry logic for rate limits

## License

This project is licensed under the MIT license
