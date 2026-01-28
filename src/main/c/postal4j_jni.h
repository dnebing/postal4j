/*
 * postal4j_jni.h
 * JNI bindings header for libpostal
 */

#ifndef POSTAL4J_JNI_H
#define POSTAL4J_JNI_H

#include <jni.h>
#include <libpostal.h>

#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_dnebinger_postal4j_LibPostal
 * Method:    setup
 * Signature: ()V
 */
 JNIEXPORT void JNICALL Java_com_dnebinger_postal4j_LibPostal_setup__
 (JNIEnv *, jclass);

/*
* Class:     com_dnebinger_postal4j_LibPostal
* Method:    setup
* Signature: (Ljava/lang/String;)V
*/
JNIEXPORT void JNICALL Java_com_dnebinger_postal4j_LibPostal_setup__Ljava_lang_String_2
 (JNIEnv *, jclass, jstring);


/*
 * Class:     com_dnebinger_postal4j_LibPostal
 * Method:    teardown
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_dnebinger_postal4j_LibPostal_teardown
  (JNIEnv *, jclass);

/*
 * Class:     com_dnebinger_postal4j_LibPostal
 * Method:    parseAddress
 * Signature: (Ljava/lang/String;)Ljava/util/Map;
 */
JNIEXPORT jobject JNICALL Java_com_dnebinger_postal4j_LibPostal_parseAddress__Ljava_lang_String_2
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_dnebinger_postal4j_LibPostal
 * Method:    parseAddress
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/util/Map;
 */
JNIEXPORT jobject JNICALL Java_com_dnebinger_postal4j_LibPostal_parseAddress__Ljava_lang_String_2Ljava_lang_String_2Ljava_lang_String_2
  (JNIEnv *, jclass, jstring, jstring, jstring);

/*
 * Class:     com_dnebinger_postal4j_LibPostal
 * Method:    expandAddress
 * Signature: (Ljava/lang/String;)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_dnebinger_postal4j_LibPostal_expandAddress__Ljava_lang_String_2
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_dnebinger_postal4j_LibPostal
 * Method:    expandAddress
 * Signature: (Ljava/lang/String;[Ljava/lang/String;ZZZZZZZZZZZZZZZZZZI)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_dnebinger_postal4j_LibPostal_expandAddress__Ljava_lang_String_2_3Ljava_lang_String_2ZZZZZZZZZZZZZZZZZZI
  (JNIEnv *, jclass, jstring, jobjectArray, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jint);

/*
 * Class:     com_dnebinger_postal4j_LibPostal
 * Method:    expandRootAddress
 * Signature: (Ljava/lang/String;)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_dnebinger_postal4j_LibPostal_expandRootAddress__Ljava_lang_String_2
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_dnebinger_postal4j_LibPostal
 * Method:    expandRootAddress
 * Signature: (Ljava/lang/String;[Ljava/lang/String;ZZZZZZZZZZZZZZZZZZI)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_dnebinger_postal4j_LibPostal_expandRootAddress__Ljava_lang_String_2_3Ljava_lang_String_2ZZZZZZZZZZZZZZZZZZI
  (JNIEnv *, jclass, jstring, jobjectArray, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jint);

#ifdef __cplusplus
}
#endif

#endif /* POSTAL4J_JNI_H */
