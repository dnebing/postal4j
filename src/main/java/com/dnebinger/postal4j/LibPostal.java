package com.dnebinger.postal4j;

import java.util.Map;

/**
 * JNI wrapper for the libpostal C library.
 * Provides address parsing, normalization, and deduplication functionality.
 */
public class LibPostal {

    static {
        NativeLibraryLoader.load("postal4j");
    }

    // Native method declarations will be added here
    public static native void setup();
    public static native void setup(String dataDir);
    public static native void teardown();

    // Address Parsing - returns label:value pairs
    public static native Map<String, String> parseAddress(String address);
    public static native Map<String, String> parseAddress(String address, String language, String country);

    // Address Expansion - returns normalized variations (using defaults)
    public static native String[] expandAddress(String address);
    public static native String[] expandAddress(String address, String[] languages, boolean latinAscii, boolean transliterate, boolean stripAccents,
        boolean decompose, boolean lowercase, boolean trimString, boolean dropParentheticals, boolean replaceNumericHyphens, boolean deleteNumericHyphens,
        boolean splitAlphaFromNumeric, boolean replaceWordHyphens, boolean deleteWordHyphens, boolean deleteFinalPeriods, boolean deleteAcronymPeriods,
        boolean dropEnglishPossessives, boolean deleteApostrophes, boolean expandNumex, boolean romanNumerals, int addressComponents);
    public static native String[] expandRootAddress(String address);
    public static native String[] expandRootAddress(String address, String[] languages, boolean latinAscii, boolean transliterate, boolean stripAccents,
        boolean decompose, boolean lowercase, boolean trimString, boolean dropParentheticals, boolean replaceNumericHyphens, boolean deleteNumericHyphens,
        boolean splitAlphaFromNumeric, boolean replaceWordHyphens, boolean deleteWordHyphens, boolean deleteFinalPeriods, boolean deleteAcronymPeriods,
        boolean dropEnglishPossessives, boolean deleteApostrophes, boolean expandNumex, boolean romanNumerals, int addressComponents);
}
