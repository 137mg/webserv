<?php
/**
 * The base configuration for WordPress
 *
 * The wp-config.php creation script uses this file during the
 * installation. You don't have to use the web site, you can
 * copy this file to "wp-config.php" and fill in the values.
 *
 * This file contains the following configurations:
 *
 * * MySQL settings
 * * Secret keys
 * * Database table prefix
 * * ABSPATH
 *
 * @link https://wordpress.org/support/article/editing-wp-config-php/
 *
 * @package WordPress
 */

// ** MySQL settings - You can get this info from your web host ** //
/** The name of the database for WordPress */
define( 'DB_NAME', 'wp_database' );

/** MySQL database username */
define( 'DB_USER', 'wp_user' );

/** MySQL database password */
define( 'DB_PASSWORD', 'wp_passwd' );

/** MySQL hostname */
define( 'DB_HOST', 'localhost' );

/** Database Charset to use in creating database tables. */
define( 'DB_CHARSET', 'utf8' );

/** The Database Collate type. Don't change this if in doubt. */
define( 'DB_COLLATE', '' );

/**#@+
 * Authentication Unique Keys and Salts.
 *
 * Change these to different unique phrases!
 * You can generate these using the {@link https://api.wordpress.org/secret-key/1.1/salt/ WordPress.org secret-key service}
 * You can change these at any point in time to invalidate all existing cookies. This will force all users to have to log in again.
 *
 * @since 2.6.0
 */

define('AUTH_KEY',         'AP%~aL=iibn3sj1l^0]YlW]Jv_1c+P|krO|B|JY9);+jZ%+De8qV!QTj*A/Xh} %');
define('SECURE_AUTH_KEY',  'ko*sEWURnes;,7k:ElTxIP/TQo.3Uc~TCa5RBRMqzOMmKmy!;`E~t$Eg(nTanxo#');
define('LOGGED_IN_KEY',    'k<-TX[o3lmo]gi0x_og7hOTdym|7|.<K)S7X,:#MSkKnIU$|?{5fBh?iNIm[-`H:');
define('NONCE_KEY',        'vfB?}VpCHT,TkR^EYQ_(v(6)icR4_[#|%(^.qEB2}nxQ,l4#|5+ i}bH~7,}f&)L');
define('AUTH_SALT',        '?H&$QB$Si.#]buAP+IKwer:U^J(*q=f[9!V=w|!?I|Y%DQ+Ni+0znb?Z(=|W?7$|');
define('SECURE_AUTH_SALT', 'taLz$3VJuti1~5#oXMWwlY5qB(onfk!_X:AdUd+ECENLO|nNSdQ xlOY85sS_BK*');
define('LOGGED_IN_SALT',   '_hqX3}LUmQvV2b@TJ8Wyz4})@$k%gO|kcmE)ExZVK7<=[8tf72C5v#.r8uf{U|>!');
define('NONCE_SALT',       '@4kY(O-J:fZ!J8ICrhJ8.CjRF$IqsY tlEdr-NZ5Pjm(6#fsZV^B:7uW9&M5#L1*');

/**#@-*/

/**
 * WordPress Database Table prefix.
 *
 * You can have multiple installations in one database if you give each
 * a unique prefix. Only numbers, letters, and underscores please!
 */
$table_prefix = 'wp_';

/**
 * For developers: WordPress debugging mode.
 *
 * Change this to true to enable the display of notices during development.
 * It is strongly recommended that plugin and theme developers use WP_DEBUG
 * in their development environments.
 *
 * For information on other constants that can be used for debugging,
 * visit the documentation.
 *
 * @link https://wordpress.org/support/article/debugging-in-wordpress/
 */
define( 'WP_DEBUG', false );

/* That's all, stop editing! Happy publishing. */

/** Absolute path to the WordPress directory. */
if ( ! defined( 'ABSPATH' ) ) {
	define( 'ABSPATH', __DIR__ . '/' );
}

/** Sets up WordPress vars and included files. */
require_once ABSPATH . 'wp-settings.php';
