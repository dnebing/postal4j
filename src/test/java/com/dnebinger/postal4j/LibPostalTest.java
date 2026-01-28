package com.dnebinger.postal4j;

import org.junit.jupiter.api.*;
import java.util.Map;

import static org.junit.jupiter.api.Assertions.*;
import static org.junit.jupiter.api.Assumptions.assumeTrue;

/**
 * Tests for the LibPostal JNI wrapper class.
 */
@TestMethodOrder(MethodOrderer.OrderAnnotation.class)
class LibPostalTest {

    private static final String DATA_DIR = "/Volumes/MinistackSSD/libpostal";
    private static boolean setupSucceeded = false;

    @Test
    @Order(1)
    void testSetupWithoutDataDir_expectFailure() {
        // This should fail because the default data directory doesn't exist
        assertThrows(RuntimeException.class, () -> LibPostal.setup());
    }

    @Test
    @Order(2)
    void testSetupWithDataDir() {
        // This should succeed with the correct data directory
        assertDoesNotThrow(() -> LibPostal.setup(DATA_DIR));
        setupSucceeded = true;
    }

    @Test
    @Order(3)
    void testParseAddressSimple() {
        assumeTrue(setupSucceeded, "Setup must succeed before running this test");

        Map<String, String> result = LibPostal.parseAddress("123 Main Street, Springfield, IL 62701");

        assertNotNull(result);
        assertFalse(result.isEmpty());

        // Print the parsed components for inspection
        System.out.println("Parsed address components:");
        result.forEach((label, value) -> System.out.println("  " + label + ": " + value));
    }

    @Test
    @Order(4)
    void testParseAddressWithLanguageAndCountry() {
        assumeTrue(setupSucceeded, "Setup must succeed before running this test");

        Map<String, String> result = LibPostal.parseAddress(
            "123 Main Street, Springfield, IL 62701",
            "en",
            "us"
        );

        assertNotNull(result);
        assertFalse(result.isEmpty());

        // Print the parsed components for inspection
        System.out.println("Parsed address components (with language/country):");
        result.forEach((label, value) -> System.out.println("  " + label + ": " + value));
    }

    @Test
    @Order(5)
    void testParseAddressInternational() {
        assumeTrue(setupSucceeded, "Setup must succeed before running this test");

        // German address
        Map<String, String> result = LibPostal.parseAddress(
            "Unter den Linden 77, 10117 Berlin, Germany",
            "de",
            "de"
        );

        assertNotNull(result);
        assertFalse(result.isEmpty());

        System.out.println("Parsed German address:");
        result.forEach((label, value) -> System.out.println("  " + label + ": " + value));
    }

    @Test
    @Order(6)
    void testExpandAddressSimple() {
        assumeTrue(setupSucceeded, "Setup must succeed before running this test");

        String[] expansions = LibPostal.expandAddress("123 Main St");

        assertNotNull(expansions);
        assertTrue(expansions.length > 0);

        System.out.println("Address expansions for '123 Main St':");
        for (String expansion : expansions) {
            System.out.println("  " + expansion);
        }
    }

    @Test
    @Order(7)
    void testExpandAddressWithOptions() {
        assumeTrue(setupSucceeded, "Setup must succeed before running this test");

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

        assertNotNull(expansions);
        assertTrue(expansions.length > 0);

        System.out.println("Address expansions with options for '123 Main St':");
        for (String expansion : expansions) {
            System.out.println("  " + expansion);
        }
    }

    @Test
    @Order(8)
    void testExpandAddressWithAbbreviation() {
        assumeTrue(setupSucceeded, "Setup must succeed before running this test");

        String[] expansions = LibPostal.expandAddress("123 E 45th St Apt 6B");

        assertNotNull(expansions);
        assertTrue(expansions.length > 0);

        System.out.println("Address expansions for '123 E 45th St Apt 6B':");
        for (String expansion : expansions) {
            System.out.println("  " + expansion);
        }
    }

    @Test
    @Order(9)
    void testExpandRootAddressSimple() {
        assumeTrue(setupSucceeded, "Setup must succeed before running this test");

        String[] expansions = LibPostal.expandRootAddress("123 Main St");

        assertNotNull(expansions);
        assertTrue(expansions.length > 0);

        System.out.println("Root address expansions for '123 Main St':");
        for (String expansion : expansions) {
            System.out.println("  " + expansion);
        }
    }

    @Test
    @Order(10)
    void testExpandRootAddressWithOptions() {
        assumeTrue(setupSucceeded, "Setup must succeed before running this test");

        String[] expansions = LibPostal.expandRootAddress(
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

        assertNotNull(expansions);
        assertTrue(expansions.length > 0);

        System.out.println("Root address expansions with options for '123 Main St':");
        for (String expansion : expansions) {
            System.out.println("  " + expansion);
        }
    }

    @Test
    @Order(11)
    void testParseAddressEmptyString() {
        assumeTrue(setupSucceeded, "Setup must succeed before running this test");

        Map<String, String> result = LibPostal.parseAddress("");

        assertNotNull(result);
        // Empty string may return empty map or minimal parsing
        System.out.println("Parsed empty address: " + result);
    }

    @Test
    @Order(12)
    void testExpandAddressEmptyString() {
        assumeTrue(setupSucceeded, "Setup must succeed before running this test");

        String[] expansions = LibPostal.expandAddress("");

        assertNotNull(expansions);
        System.out.println("Expansions for empty string: " + expansions.length + " results");
    }

    @Test
    @Order(100)
    void testTeardown() {
        assumeTrue(setupSucceeded, "Setup must succeed before running teardown");

        assertDoesNotThrow(() -> LibPostal.teardown());
    }
}
