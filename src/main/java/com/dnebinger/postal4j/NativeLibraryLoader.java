package com.dnebinger.postal4j;

import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardCopyOption;

/**
 * Utility class for loading native libraries from the classpath or system paths.
 */
public final class NativeLibraryLoader {

    private static final String OS_NAME = System.getProperty("os.name").toLowerCase();
    private static final String OS_ARCH = System.getProperty("os.arch").toLowerCase();

    private NativeLibraryLoader() {
        // Utility class
    }

    /**
     * Loads the specified native library.
     * First attempts to load from the system library path, then falls back to
     * extracting from the classpath if not found.
     *
     * @param libraryName the name of the library to load (without platform-specific prefix/suffix)
     */
    public static void load(String libraryName) {
        try {
            // First, try loading from system library path
            System.loadLibrary(libraryName);
        } catch (UnsatisfiedLinkError e) {
            // Fall back to loading from classpath
            loadFromClasspath(libraryName);
        }
    }

    private static void loadFromClasspath(String libraryName) {
        String resourcePath = getNativeResourcePath(libraryName);

        try (InputStream in = NativeLibraryLoader.class.getResourceAsStream(resourcePath)) {
            if (in == null) {
                throw new UnsatisfiedLinkError(
                    "Native library not found: " + resourcePath +
                    " (OS: " + getOsName() + ", Arch: " + getArchName() + ")");
            }

            // Create a temporary file with the correct extension
            String extension = getLibraryExtension();
            Path tempFile = Files.createTempFile(libraryName, extension);
            tempFile.toFile().deleteOnExit();

            // Copy the library to the temp file
            Files.copy(in, tempFile, StandardCopyOption.REPLACE_EXISTING);

            // Load the library from the temp file
            System.load(tempFile.toAbsolutePath().toString());
        } catch (IOException e) {
            throw new UnsatisfiedLinkError("Failed to extract native library: " + e.getMessage());
        }
    }

    private static String getNativeResourcePath(String libraryName) {
        String osName = getOsName();
        String archName = getArchName();
        String libFileName = getLibraryFileName(libraryName);

        return "/native/" + osName + "-" + archName + "/" + libFileName;
    }

    private static String getOsName() {
        if (OS_NAME.contains("mac") || OS_NAME.contains("darwin")) {
            return "darwin";
        } else if (OS_NAME.contains("linux")) {
            return "linux";
        } else if (OS_NAME.contains("win")) {
            return "windows";
        }
        return OS_NAME;
    }

    private static String getArchName() {
        if (OS_ARCH.contains("aarch64") || OS_ARCH.contains("arm64")) {
            return "aarch64";
        } else if (OS_ARCH.contains("amd64") || OS_ARCH.contains("x86_64")) {
            return "x86_64";
        }
        return OS_ARCH;
    }

    private static String getLibraryFileName(String libraryName) {
        if (OS_NAME.contains("mac") || OS_NAME.contains("darwin")) {
            return "lib" + libraryName + ".dylib";
        } else if (OS_NAME.contains("win")) {
            return libraryName + ".dll";
        } else {
            // Linux and others
            return "lib" + libraryName + ".so";
        }
    }

    private static String getLibraryExtension() {
        if (OS_NAME.contains("mac") || OS_NAME.contains("darwin")) {
            return ".dylib";
        } else if (OS_NAME.contains("win")) {
            return ".dll";
        } else {
            return ".so";
        }
    }
}
