/**
 * Returns the major component of the library version.
 */
unsigned char get_version_major(void);

/**
 * Returns the minor component of the library version.
 */
unsigned char get_version_minor(void);

/**
 * Returns the patch component of the library version.
 */
unsigned char get_version_patch(void);

/**
 * Returns the semantic version of the library in the form Major.Minor.Patch
 */
char const* get_version(void);
