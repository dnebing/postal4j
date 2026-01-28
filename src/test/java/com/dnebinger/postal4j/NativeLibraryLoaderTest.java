package com.dnebinger.postal4j;

import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

/**
 * Tests for the NativeLibraryLoader class.
 */
class NativeLibraryLoaderTest {

    @Test
    void testLoadLibrary() {
        // This should not throw an exception - the library should load successfully
        assertDoesNotThrow(() -> NativeLibraryLoader.load("postal4j"));
    }

    @Test
    void testLoadNonExistentLibrary() {
        // Loading a non-existent library should throw UnsatisfiedLinkError
        assertThrows(UnsatisfiedLinkError.class, () -> NativeLibraryLoader.load("nonexistent_library_12345"));
    }

    @Test
    void testLoadLibraryMultipleTimes() {
        // Loading the same library multiple times should be safe (no-op after first load)
        assertDoesNotThrow(() -> {
            NativeLibraryLoader.load("postal4j");
            NativeLibraryLoader.load("postal4j");
        });
    }
}
