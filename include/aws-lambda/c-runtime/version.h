/**
 * Returns the major component of the library version.
 */
unsigned char get_version_major();

/**
 * Returns the minor component of the library version.
 */
unsigned char get_version_minor();

/**
 * Returns the patch component of the library version.
 */
unsigned char get_version_patch();

/**
 * Returns the semantic version of the library in the form Major.Minor.Patch
 */
char const* get_version();
