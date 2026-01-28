# postal4j

A Java JNI wrapper for [libpostal](https://github.com/openvenues/libpostal), the fast statistical parser/normalizer for street addresses.

## Overview

postal4j provides Java bindings for libpostal, allowing you to:

- **Parse addresses** into structured components (house number, street, city, state, postal code, country, etc.)
- **Expand/normalize addresses** to canonical forms (e.g., "123 Main St" → "123 main street")
- Support for international addresses in multiple languages

This is a rework of previous jpostal JNI wrappers, designed to be simpler and more compatible with modern Java frameworks like Quarkus (dev, JVM, or native mode).

## Requirements

- Java 17 or higher
- Gradle 8.14+
- libpostal installed on your system with data files downloaded
- A C compiler (gcc or clang)

### Installing libpostal

Follow the [libpostal installation instructions](https://github.com/openvenues/libpostal#installation) for your platform.

**macOS (Homebrew):**
```bash
brew install libpostal
```

**Linux (from source):**
```bash
git clone https://github.com/openvenues/libpostal
cd libpostal
./bootstrap.sh
./configure --datadir=/path/to/data
make -j4
sudo make install
sudo ldconfig  # Linux only
```

After installation, download the data files:
```bash
libpostal_data download all /path/to/data
```

## Building

```bash
# Clone the repository
git clone https://github.com/dnebing/postal4j.git
cd postal4j

# Build the project (compiles Java and native library)
./gradlew build

# Build only the native shared library
./gradlew postal4jSharedLibrary

# Build only the Java JAR
./gradlew jar
```

The build produces:
- `build/libs/postal4j-1.0.0-SNAPSHOT.jar` - Java library with bundled native code
- `build/libs/postal4j/shared/libpostal4j.dylib` (macOS) or `libpostal4j.so` (Linux)

## Usage

### Basic Setup

```java
import com.dnebinger.postal4j.LibPostal;

// Initialize with default data directory
LibPostal.setup();

// Or specify a custom data directory
LibPostal.setup("/path/to/libpostal/data");

// Always call teardown when done
LibPostal.teardown();
```

### Parsing Addresses

```java
import java.util.Map;

// Parse an address into components
Map<String, String> components = LibPostal.parseAddress(
    "123 Main Street, Springfield, IL 62701"
);

// Returns: {house_number=123, road=main street, city=springfield, state=il, postcode=62701}

// Parse with language and country hints
Map<String, String> components = LibPostal.parseAddress(
    "Unter den Linden 77, 10117 Berlin, Germany",
    "de",  // language
    "de"   // country
);
```

### Expanding/Normalizing Addresses

```java
// Get normalized variations of an address
String[] expansions = LibPostal.expandAddress("123 Main St Apt 4B");

// Returns variations like:
// - "123 main street apartment 4b"
// - "123 main saint apartment 4b"
// - "123 main street apt 4b"
// etc.

// Expand with custom options
String[] expansions = LibPostal.expandAddress(
    "123 Main St",
    new String[]{"en"},  // languages
    true,   // latinAscii
    true,   // transliterate
    true,   // stripAccents
    true,   // decompose
    true,   // lowercase
    true,   // trimString
    false,  // dropParentheticals
    false,  // replaceNumericHyphens
    false,  // deleteNumericHyphens
    false,  // splitAlphaFromNumeric
    false,  // replaceWordHyphens
    false,  // deleteWordHyphens
    false,  // deleteFinalPeriods
    false,  // deleteAcronymPeriods
    false,  // dropEnglishPossessives
    false,  // deleteApostrophes
    true,   // expandNumex
    false,  // romanNumerals
    0xFFFF  // addressComponents (all)
);
```

### Root Address Expansion

```java
// Get root expansions (fewer variations, more canonical)
String[] rootExpansions = LibPostal.expandRootAddress("123 Main St");
```

## API Reference

### LibPostal

| Method | Description |
|--------|-------------|
| `setup()` | Initialize libpostal with default data directory |
| `setup(String dataDir)` | Initialize with custom data directory |
| `teardown()` | Release libpostal resources |
| `parseAddress(String address)` | Parse address into labeled components |
| `parseAddress(String address, String language, String country)` | Parse with language/country hints |
| `expandAddress(String address)` | Get normalized address variations |
| `expandAddress(String address, String[] languages, ...)` | Expand with custom options |
| `expandRootAddress(String address)` | Get root/canonical expansions |
| `expandRootAddress(String address, String[] languages, ...)` | Root expand with options |

### Address Components

The parser returns a `Map<String, String>` with keys that may include:

- `house` - Venue name (e.g., "Empire State Building")
- `house_number` - House/building number
- `road` - Street name
- `unit` - Apartment/unit number
- `level` - Floor number
- `staircase` - Staircase identifier
- `entrance` - Entrance identifier
- `po_box` - PO Box number
- `postcode` - Postal/ZIP code
- `suburb` - Suburb/neighborhood
- `city_district` - City district
- `city` - City name
- `island` - Island name
- `state_district` - State district
- `state` - State/province
- `country_region` - Country region
- `country` - Country name
- `world_region` - World region

**See the `libpostal` repo for all possible keys to expect.**

## Testing

```bash
# Run all tests
./gradlew test

# Run with verbose output
./gradlew test --info
```

Note: Tests require libpostal data files. Update the `DATA_DIR` constant in `LibPostalTest.java` to point to your data directory.

## Project Structure

```
postal4j/
├── src/
│   ├── main/
│   │   ├── java/com/dnebinger/postal4j/
│   │   │   ├── LibPostal.java           # Main JNI wrapper class
│   │   │   └── NativeLibraryLoader.java # Native library loader
│   │   └── c/
│   │       ├── postal4j_jni.h           # JNI header
│   │       └── postal4j_jni.c           # JNI implementation
│   └── test/
│       └── java/com/dnebinger/postal4j/
│           ├── LibPostalTest.java
│           └── NativeLibraryLoaderTest.java
├── build.gradle                          # Gradle build configuration
├── settings.gradle
├── libpostal.h                           # libpostal header (reference)
└── README.md
```

## Gradle Tasks

| Task | Description |
|------|-------------|
| `./gradlew build` | Full build (Java + native) |
| `./gradlew jar` | Build the Java JAR |
| `./gradlew postal4jSharedLibrary` | Build native shared library (.so/.dylib/.dll) |
| `./gradlew postal4jStaticLibrary` | Build native static library |
| `./gradlew generateJniHeaders` | Generate JNI headers from Java native methods |
| `./gradlew test` | Run tests |
| `./gradlew clean` | Clean build artifacts |

## How It Works

1. **Native Library Loading**: When `LibPostal` is first accessed, `NativeLibraryLoader` attempts to load the native library:
   - First tries `System.loadLibrary()` for system-installed libraries
   - Falls back to extracting the bundled library from the JAR to a temp file

2. **JNI Bridge**: The C code in `postal4j_jni.c` bridges Java calls to libpostal:
   - Converts Java strings to C strings
   - Calls libpostal functions
   - Converts results back to Java objects (Maps, String arrays)
   - Handles memory management and error propagation

3. **Build Process**:
   - Gradle compiles Java sources
   - The `c` plugin compiles native code and links against libpostal
   - Native library is bundled into the JAR under `native/{os}-{arch}/`

## Platform Support

| Platform | Architecture | Status |
|----------|--------------|--------|
| macOS | aarch64 (Apple Silicon) | Supported |
| macOS | x86_64 | Supported |
| Linux | x86_64 | Supported |
| Linux | aarch64 | Supported |
| Windows | x86_64 | Untested |

## Quarkus Integration

This library is designed to work with Quarkus in dev, JVM, and native modes. When building Quarkus native images, you'll need to link both `postal4j` and `libpostal`:

```properties
# application.properties
quarkus.native.additional-build-args=-H:AdditionalLinkedLibraries=postal4j,postal
```

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.

libpostal itself is licensed under the MIT License.

## Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.

## Acknowledgments

- [libpostal](https://github.com/openvenues/libpostal) by Al Barrentine and the OpenVenues team
- Built with [Gradle](https://gradle.org/) and the native software model
