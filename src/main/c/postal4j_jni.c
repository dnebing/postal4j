/*
 * postal4j_jni.c
 * JNI bindings implementation for libpostal
 */

#include "postal4j_jni.h"
#include <stdlib.h>
#include <string.h>

// Forward declarations for helper functions
void throwException(JNIEnv *env, const char *message);
jobject parseAddressWithOptions(JNIEnv *env, char* address, libpostal_address_parser_options_t options);
jobjectArray expandAddressWithOptions(JNIEnv *env, char* address, libpostal_normalize_options_t options);
jobjectArray expandRootAddressWithOptions(JNIEnv *env, char* address, libpostal_normalize_options_t options);
libpostal_normalize_options_t* mallocNormalizeOptions(JNIEnv *env, jobjectArray languages, jboolean latinAscii, jboolean transliterate,
    jboolean stripAccents, jboolean decompose, jboolean lowercase, jboolean trimString, jboolean dropParentheticals, jboolean replaceNumericHyphens,
    jboolean deleteNumericHyphens, jboolean splitAlphaFromNumeric, jboolean replaceWordHyphens, jboolean deleteWordHyphens,
    jboolean deleteFinalPeriods, jboolean deleteAcronymPeriods, jboolean dropEnglishPossessives, jboolean deleteApostrophes, jboolean expandNumex,
    jboolean romanNumerals, jint addressComponents);
void freeNormalizeOptions(libpostal_normalize_options_t* options);
jobjectArray createResultArray(JNIEnv *env, char** expansions, size_t numExpansions);

/*
 * Class:     com_dnebinger_postal4j_LibPostal
 * Method:    setup
 * Signature: ()Z
 */
JNIEXPORT void JNICALL Java_com_dnebinger_postal4j_LibPostal_setup__
  (JNIEnv *env, jclass cls) {
    // initialize libpostal first
    if (!libpostal_setup()) {
        throwException(env, "Error initializing libpostal");
        return;
    }

    // next initialize the parser
    if (!libpostal_setup_parser()) {
        throwException(env, "Error initializing libpostal parser");
        return;
    }

    // finally initialize the language classifier
    if (!libpostal_setup_language_classifier()) {
        throwException(env, "Error initializing libpostal language classifier");
        return;
    }
}

/*
 * Class:     com_dnebinger_postal4j_LibPostal
 * Method:    setup
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT void JNICALL Java_com_dnebinger_postal4j_LibPostal_setup__Ljava_lang_String_2
  (JNIEnv *env, jclass cls, jstring dataDir) {
    // extract the data directory from the JNI string
    const char *dataDirStr = (*env)->GetStringUTFChars(env, dataDir, NULL);

    if (dataDirStr == NULL) {
        throwException(env, "Error extracting data directory");
        return;
    }

    // initialize libpostal with the data directory
    if (!libpostal_setup_datadir((char*)dataDirStr)) {
        throwException(env, "Error initializing libpostal with data directory");
        (*env)->ReleaseStringUTFChars(env, dataDir, dataDirStr);
        return;
    }

    // initialize the parser with the data directory
    if (!libpostal_setup_parser_datadir((char*)dataDirStr)) {
        throwException(env, "Error initializing libpostal parser with data directory");
        (*env)->ReleaseStringUTFChars(env, dataDir, dataDirStr);
        return;
    }

    // initialize the language classifier with the data directory
    if (!libpostal_setup_language_classifier_datadir((char*)dataDirStr)) {
        throwException(env, "Error initializing libpostal language classifier with data directory");
        (*env)->ReleaseStringUTFChars(env, dataDir, dataDirStr);
        return;
    }

    // free the data directory string
    (*env)->ReleaseStringUTFChars(env, dataDir, dataDirStr);
}

/*
 * Class:     com_dnebinger_postal4j_LibPostal
 * Method:    teardown
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_dnebinger_postal4j_LibPostal_teardown
  (JNIEnv *env, jclass cls) {
    // reverse order teardown of the modules

    libpostal_teardown_language_classifier();
    libpostal_teardown_parser();
    libpostal_teardown();
}

/*
 * Class:     com_dnebinger_postal4j_LibPostal
 * Method:    parseAddress
 * Signature: (Ljava/lang/String;)Ljava/util/Map;
 */
JNIEXPORT jobject JNICALL Java_com_dnebinger_postal4j_LibPostal_parseAddress__Ljava_lang_String_2
  (JNIEnv *env, jclass cls, jstring jaddress) {

    // extract the address from the JNI string
    const char *address = (*env)->GetStringUTFChars(env, jaddress, 0);

    // check if the address is null
    if (address == NULL) {
        throwException(env, "Error extracting address");
        return NULL;
    }

    // initialize the address parser options
    libpostal_address_parser_options_t options = libpostal_get_address_parser_default_options();

    jobject resultMap = parseAddressWithOptions(env, (char*)address, options);

    // free the address string
    (*env)->ReleaseStringUTFChars(env, jaddress, address);

    // return the result map
    return resultMap;
}

/*
 * Helper function to parse an address with options
 * @param env the JNI environment
 * @param address the address string
 * @param options the address parser options
 * @return the result map
 */
jobject parseAddressWithOptions(JNIEnv *env, char* address, libpostal_address_parser_options_t options) {
    // parse the address
    libpostal_address_parser_response_t *response = libpostal_parse_address(address, options);

    if (response == NULL) {
        throwException(env, "Error parsing address");
        return NULL;
    }

    // Create HashMap<String, String> directly that we will return to the caller
    jclass hashMapClass = (*env)->FindClass(env, "java/util/HashMap");
    jmethodID hashMapInit = (*env)->GetMethodID(env, hashMapClass, "<init>", "()V");
    jmethodID hashMapPut = (*env)->GetMethodID(env, hashMapClass, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
    jobject resultMap = (*env)->NewObject(env, hashMapClass, hashMapInit);
    
    // populate the hash map with the address components
    for (size_t i = 0; i < response->num_components; i++) {
        // create new strings for the label and value
        jstring jlabel = (*env)->NewStringUTF(env, response->labels[i]);
        jstring jvalue = (*env)->NewStringUTF(env, response->components[i]);

        // put the label and value into the hash map
        (*env)->CallObjectMethod(env, resultMap, hashMapPut, jlabel, jvalue);
    }

    // done with the response
    libpostal_address_parser_response_destroy(response);

    // return the result map
    return resultMap;
}

/*
 * Class:     com_dnebinger_postal4j_LibPostal
 * Method:    parseAddress
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/util/Map;
 */
JNIEXPORT jobject JNICALL Java_com_dnebinger_postal4j_LibPostal_parseAddress__Ljava_lang_String_2Ljava_lang_String_2Ljava_lang_String_2
  (JNIEnv *env, jclass cls, jstring jaddress, jstring jlanguage, jstring jcountry) {
    // extract the address from the JNI string
    const char *address = (*env)->GetStringUTFChars(env, jaddress, 0);

    // check if the address is null
    if (address == NULL) {
        throwException(env, "Error extracting address");
        return NULL;
    }

    // extract language and country
    const char *language = (*env)->GetStringUTFChars(env, jlanguage, NULL);
    const char *country = (*env)->GetStringUTFChars(env, jcountry, NULL);

    // create options struct with the language and country
    libpostal_address_parser_options_t* options = (libpostal_address_parser_options_t*) malloc(sizeof(libpostal_address_parser_options_t));

    options->language = (char*)language;
    options->country = (char*)country;

    // parse the address
    jobject resultMap = parseAddressWithOptions(env, (char*)address, *options);

    // free the strings
    (*env)->ReleaseStringUTFChars(env, jlanguage, language);
    (*env)->ReleaseStringUTFChars(env, jcountry, country);
    (*env)->ReleaseStringUTFChars(env, jaddress, address);

    // free the address parser options
    free(options);

    // return the result map
    return resultMap;
}

/*
 * Class:     com_dnebinger_postal4j_LibPostal
 * Method:    expandAddress
 * Signature: (Ljava/lang/String;)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_dnebinger_postal4j_LibPostal_expandAddress__Ljava_lang_String_2
  (JNIEnv *env, jclass cls, jstring jaddress) {
    // extract the address from the JNI string
    const char *address = (*env)->GetStringUTFChars(env, jaddress, 0);

    // check if the address is null
    if (address == NULL) {
        throwException(env, "Error extracting address");
        return NULL;
    }

    // get the default normalize options
    libpostal_normalize_options_t options = libpostal_get_default_options();

    // expand the address
    jobjectArray resultArray = expandAddressWithOptions(env, (char*)address, options);

    // free the address string
    (*env)->ReleaseStringUTFChars(env, jaddress, address);

    // return the result array
    return resultArray;
}

/*
 * Helper function to expand an address with options
 * @param env the JNI environment
 * @param address the address string
 * @param options the normalize options
 * @return the result array
 */
jobjectArray expandAddressWithOptions(JNIEnv *env, char* address, libpostal_normalize_options_t options) {
    // expand the address
    size_t numExpansions;
    char **expansions = libpostal_expand_address(address, options, &numExpansions);

    return createResultArray(env, expansions, numExpansions);
}

/*
 * Class:     com_dnebinger_postal4j_LibPostal
 * Method:    expandAddress
 * Signature: (Ljava/lang/String;[Ljava/lang/String;ZZZZZZZZZZZZZZZZZZI)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_dnebinger_postal4j_LibPostal_expandAddress__Ljava_lang_String_2_3Ljava_lang_String_2ZZZZZZZZZZZZZZZZZZI
  (JNIEnv *env, jclass cls, jstring jaddress, jobjectArray languages,
   jboolean latinAscii, jboolean transliterate, jboolean stripAccents,
   jboolean decompose, jboolean lowercase, jboolean trimString,
   jboolean dropParentheticals, jboolean replaceNumericHyphens,
   jboolean deleteNumericHyphens, jboolean splitAlphaFromNumeric,
   jboolean replaceWordHyphens, jboolean deleteWordHyphens,
   jboolean deleteFinalPeriods, jboolean deleteAcronymPeriods,
   jboolean dropEnglishPossessives, jboolean deleteApostrophes,
   jboolean expandNumex, jboolean romanNumerals, jint addressComponents) {

    // extract the address from the JNI string
    const char *address = (*env)->GetStringUTFChars(env, jaddress, 0);

    // check if the address is null
    if (address == NULL) {
        throwException(env, "Error extracting address");
        return NULL;
    }

    // create the normalize options struct
    libpostal_normalize_options_t* options = mallocNormalizeOptions(env, languages, latinAscii, transliterate, stripAccents, decompose, lowercase, trimString,
        dropParentheticals, replaceNumericHyphens, deleteNumericHyphens, splitAlphaFromNumeric, replaceWordHyphens, deleteWordHyphens, deleteFinalPeriods,
        deleteAcronymPeriods, dropEnglishPossessives, deleteApostrophes, expandNumex, romanNumerals, addressComponents);

    // expand the address
    jobjectArray resultArray = expandAddressWithOptions(env, (char*)address, *options);

    // free the normalize options
    freeNormalizeOptions(options);

    // free the address string
    (*env)->ReleaseStringUTFChars(env, jaddress, address);

    // return the result array
    return resultArray;
}

/*
 * Class:     com_dnebinger_postal4j_LibPostal
 * Method:    expandRootAddress
 * Signature: (Ljava/lang/String;)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_dnebinger_postal4j_LibPostal_expandRootAddress__Ljava_lang_String_2
  (JNIEnv *env, jclass cls, jstring jaddress) {
    // extract the address from the JNI string
    const char *address = (*env)->GetStringUTFChars(env, jaddress, 0);

    // check if the address is null
    if (address == NULL) {
        throwException(env, "Error extracting address");
        return NULL;
    }

    // get the default normalize options
    libpostal_normalize_options_t options = libpostal_get_default_options();

    // expand the address
    jobjectArray resultArray = expandRootAddressWithOptions(env, (char*)address, options);

    // free the address string
    (*env)->ReleaseStringUTFChars(env, jaddress, address);

    // return the result array
    return resultArray;
}

/*
 * Helper function to expand a root address with options
 * @param env the JNI environment
 * @param address the address string
 * @param options the normalize options
 * @return the result array
 */
jobjectArray expandRootAddressWithOptions(JNIEnv *env, char* address, libpostal_normalize_options_t options) {
    // expand the address
    size_t numExpansions;
    char **expansions = libpostal_expand_address_root(address, options, &numExpansions);

    return createResultArray(env, expansions, numExpansions);
}

/*
 * Helper function to create a result array
 * @param env the JNI environment
 * @param expansions the expansions
 * @param numExpansions the number of expansions
 * @return the result array
 */
jobjectArray createResultArray(JNIEnv *env, char** expansions, size_t numExpansions) {
    if (expansions == NULL) {
        throwException(env, "Error expanding root address");
        return NULL;
    }

    // create the result array
    jclass stringClass = (*env)->FindClass(env, "java/lang/String");
    jobjectArray resultArray = (*env)->NewObjectArray(env, numExpansions, stringClass, NULL);

    // populate the result array
    for (size_t i = 0; i < numExpansions; i++) {
        // create new string for the expansion
        jstring jexpansion = (*env)->NewStringUTF(env, expansions[i]);

        // set the expansion in the result array
        (*env)->SetObjectArrayElement(env, resultArray, i, jexpansion);
    }

    // done with the expansions
    libpostal_expansion_array_destroy(expansions, numExpansions);

    // return the result array
    return resultArray;
}

/*
 * Class:     com_dnebinger_postal4j_LibPostal
 * Method:    expandRootAddress
 * Signature: (Ljava/lang/String;[Ljava/lang/String;ZZZZZZZZZZZZZZZZZZI)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_dnebinger_postal4j_LibPostal_expandRootAddress__Ljava_lang_String_2_3Ljava_lang_String_2ZZZZZZZZZZZZZZZZZZI
  (JNIEnv *env, jclass cls, jstring jaddress, jobjectArray languages,
   jboolean latinAscii, jboolean transliterate, jboolean stripAccents,
   jboolean decompose, jboolean lowercase, jboolean trimString,
   jboolean dropParentheticals, jboolean replaceNumericHyphens,
   jboolean deleteNumericHyphens, jboolean splitAlphaFromNumeric,
   jboolean replaceWordHyphens, jboolean deleteWordHyphens,
   jboolean deleteFinalPeriods, jboolean deleteAcronymPeriods,
   jboolean dropEnglishPossessives, jboolean deleteApostrophes,
   jboolean expandNumex, jboolean romanNumerals, jint addressComponents) {

    // extract the address from the JNI string
    const char *address = (*env)->GetStringUTFChars(env, jaddress, 0);

    // check if the address is null
    if (address == NULL) {
        throwException(env, "Error extracting address");
        return NULL;
    }

    // create the normalize options
    libpostal_normalize_options_t* options = mallocNormalizeOptions(env, languages, latinAscii, transliterate, stripAccents, decompose, lowercase, trimString,
        dropParentheticals, replaceNumericHyphens, deleteNumericHyphens, splitAlphaFromNumeric, replaceWordHyphens, deleteWordHyphens, deleteFinalPeriods,
        deleteAcronymPeriods, dropEnglishPossessives, deleteApostrophes, expandNumex, romanNumerals, addressComponents);

    // expand the address
    jobjectArray resultArray = expandRootAddressWithOptions(env, (char*)address, *options);

    // free the address string
    (*env)->ReleaseStringUTFChars(env, jaddress, address);

    // free the normalize options
    freeNormalizeOptions(options);

    // return the result array
    return resultArray;
}

/*
 * Helper function to create a normalize options struct
 * @param env the JNI environment
 * @param languages the languages
 * @param latinAscii the latin ascii flag
 * @param transliterate the transliterate flag
 * @param stripAccents the strip accents flag
 * @param decompose the decompose flag
 * @param lowercase the lowercase flag
 * @param trimString the trim string flag
 * @param dropParentheticals the drop parentheticals flag
 * @param replaceNumericHyphens the replace numeric hyphens flag
 * @param deleteNumericHyphens the delete numeric hyphens flag
 * @param splitAlphaFromNumeric the split alpha from numeric flag
 * @param replaceWordHyphens the replace word hyphens flag
 * @param deleteWordHyphens the delete word hyphens flag
 * @param deleteFinalPeriods the delete final periods flag
 * @param deleteAcronymPeriods the delete acronym periods flag
 * @param dropEnglishPossessives the drop english possessives flag
 * @param deleteApostrophes the delete apostrophes flag
 * @param expandNumex the expand numex flag
 * @param romanNumerals the roman numerals flag
 * @param addressComponents the address components flag
 * @return the normalize options struct
 */
libpostal_normalize_options_t* mallocNormalizeOptions(JNIEnv *env, jobjectArray languages, jboolean latinAscii, jboolean transliterate, jboolean stripAccents, 
    jboolean decompose, jboolean lowercase, jboolean trimString, jboolean dropParentheticals, jboolean replaceNumericHyphens, jboolean deleteNumericHyphens, 
    jboolean splitAlphaFromNumeric, jboolean replaceWordHyphens, jboolean deleteWordHyphens, jboolean deleteFinalPeriods, jboolean deleteAcronymPeriods, 
    jboolean dropEnglishPossessives, jboolean deleteApostrophes, jboolean expandNumex, jboolean romanNumerals, jint addressComponents) {
    // create the normalize options struct
    libpostal_normalize_options_t* options = (libpostal_normalize_options_t*) malloc(sizeof(libpostal_normalize_options_t));

    if (languages != NULL) {
        // set the languages
        options->num_languages = (*env)->GetArrayLength(env, languages);

        if (options->num_languages > 0) {
            // allocate memory for the languages
            char** c_languages = malloc(options->num_languages * sizeof(char*));

            // populate the languages
            for (int i = 0; i < options->num_languages; i++) {
                // get the language from the JNI array
                jstring jlang = (*env)->GetObjectArrayElement(env, languages, i);

                // convert the language to a UTF-8 string
                const char* lang_utf = (*env)->GetStringUTFChars(env, jlang, NULL);

                // make a copy of the language
                c_languages[i] = strdup(lang_utf);

                // release the language string
                (*env)->ReleaseStringUTFChars(env, jlang, lang_utf);
            }

            // set the languages in the options struct
            options->languages = c_languages;
        }
        else {
            options->num_languages = 0;
            options->languages = NULL;
        }
    } else {
        options->num_languages = 0;
        options->languages = NULL;
    }

    // set the other options
    options->latin_ascii = latinAscii;
    options->transliterate = transliterate;
    options->strip_accents = stripAccents;
    options->decompose = decompose;
    options->lowercase = lowercase;
    options->trim_string = trimString;
    options->drop_parentheticals = dropParentheticals;
    options->replace_numeric_hyphens = replaceNumericHyphens;
    options->delete_numeric_hyphens = deleteNumericHyphens;
    options->split_alpha_from_numeric = splitAlphaFromNumeric;
    options->replace_word_hyphens = replaceWordHyphens;
    options->delete_word_hyphens = deleteWordHyphens;
    options->delete_final_periods = deleteFinalPeriods;
    options->delete_acronym_periods = deleteAcronymPeriods;
    options->drop_english_possessives = dropEnglishPossessives;
    options->delete_apostrophes = deleteApostrophes;
    options->expand_numex = expandNumex;
    options->roman_numerals = romanNumerals;
    options->address_components = addressComponents;

    return options;
}

/*
 * Helper function to free a normalize options struct
 * @param options the normalize options struct
 */
void freeNormalizeOptions(libpostal_normalize_options_t* options) {
    if (options == NULL) {
        return;
    }

    if (options->num_languages > 0) {
    // free the languages
        for (int i = 0; i < options->num_languages; i++) {
            free(options->languages[i]);
        }
        free(options->languages);
    }

    // free the options struct
    free(options);
}

/*
 * Helper function to throw an exception
 * @param env the JNI environment
 * @param message the message
 */
void throwException(JNIEnv *env, const char *message) {
    jclass exceptionClass  = (*env)->FindClass(env, "java/lang/RuntimeException");
    if (exceptionClass == NULL) {
        return;
    }

    // throw the exception
    (*env)->ThrowNew(env, exceptionClass, message);
}

