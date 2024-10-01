<?php

/**
 * @file
 */

use Drupal\Core\Url;

/**
 * @file
 * Admin page callbacks for the boost module.
 */

/**
 * Default apache server name.
 */
define('BOOST_SERVER_NAME_HTTP_HOST', '%{HTTP_HOST}');

/**
 * Default apache document root.
 */
define('BOOST_DOCUMENT_ROOT', '%{DOCUMENT_ROOT}');

/**
 * Default setting for SSL pages.
 */
define('BOOST_SSL_BYPASS', TRUE);

/**
 * Form builder; Configure boost settings.
 *
 * @ingroup forms
 * @see system_settings_form()
 */
function boost_admin_htaccess_settings() {
  global $base_path;

  // Apache .htaccess settings generation
  //   $htaccess = boost_admin_generate_htaccess();
  // @FIXME
  // url() expects a route name or an external URI.
  // $form['htaccess'] = array(
  //     '#type'          => 'fieldset',
  //     '#title'         => t('Boost Apache .htaccess settings generation'),
  //     '#description'   => t('<a href="!link">Explanation of .htaccess variables</a> <br /><br />  <em>Be sure to save the configuration and then go to the <a href="!rules">htaccess rules generation page</a> and copy the rules.</em> <br /><strong>Apache 2.4 users should uncomment the two marked sections, each line beginning with #</strong> ', array('!link' => \Drupal\Core\Url::fromUri('http://www.askapache.com/htaccess/mod_rewrite-variables-cheatsheet.html'), '!rules' => url('admin/config/system/boost/htaccess/generator'))),
  //   );

  // @FIXME
  // Could not extract the default value because it is either indeterminate, or
  // not scalar. You'll need to provide a default value in
  // config/install/boost.settings.yml and config/schema/boost.schema.yml.
  $form['htaccess']['boost_server_name_http_host'] = [
    '#type' => 'radios',
    '#title' => t("Server's URL or Name"),
    '#default_value' => \Drupal::config('boost.settings')->get('boost_server_name_http_host'),
    '#options' => [
      '%{HTTP_HOST}' => '%{HTTP_HOST}',
      '%{SERVER_NAME}' => '%{SERVER_NAME}',
      $_SERVER['HTTP_HOST'] => $_SERVER['HTTP_HOST'],
      $_SERVER['SERVER_NAME'] => $_SERVER['SERVER_NAME'],
    ],
    '#description' => t('Best to leave these as %{}, only try the last option(s) if boost is still not working.'),
  ];
  // Set DOCUMENT_ROOT
  $drupal_subdir = rtrim($base_path, '/');
  // fix windows dir slashes
  $document_root = str_replace('\\', '/', getcwd());
  // remove subdir
  $document_root = trim(str_replace($drupal_subdir, '', $document_root));
  // initial options
  $options = ['%{DOCUMENT_ROOT}' => '%{DOCUMENT_ROOT}', $document_root => $document_root];
  // values to ignore
  $rejects = ['SCRIPT_FILENAME', 'DOCUMENT_ROOT'];
  // search for values that match getcwd
  $output = boost_admin_htaccess_array_find($document_root, $_SERVER, $rejects);
  $description_extra = '';
  if (!empty($output)) {
    foreach ($output as $key => $value) {
      $temp = '%{ENV:' . $key . '}';
      // adding values to options
      $options[$temp] = $temp . ' = ' . $value;
      if (strcmp($value, $document_root) == 0) {
        // set best since it's a match
        $best = $temp;
      }
    }
  }
  if (strcmp($_SERVER['DOCUMENT_ROOT'], $document_root) == 0) {
    $best = '%{DOCUMENT_ROOT}';
  }
  elseif (!isset($best)) {
    $best = $document_root;
    $description_extra = t('Please <a href="!link">open an boost issue on Drupal.org</a>, since apache and php might not be configured correctly.', [
      '!link' => Url::fromUri('http://drupal.org/node/add/project-issue/boost'),
    ]);
  }
  $percent = 0;
  $int = similar_text(substr(trim($_SERVER['DOCUMENT_ROOT']), 18, 1), substr(trim($document_root), 18, 1), $percent);
  $description = t('Value of %best is recommended for this server.', ['%best' => $best]) . ' ' . $description_extra;
  // @FIXME
  // Could not extract the default value because it is either indeterminate, or
  // not scalar. You'll need to provide a default value in
  // config/install/boost.settings.yml and config/schema/boost.schema.yml.
  $form['htaccess']['boost_document_root'] = [
    '#type' => 'radios',
    '#title' => t('Document Root'),
    '#default_value' => \Drupal::config('boost.settings')->get('boost_document_root'),
    '#options' => $options,
    '#description' => $description,
  ];
  // @FIXME
  // Could not extract the default value because it is either indeterminate, or
  // not scalar. You'll need to provide a default value in
  // config/install/boost.settings.yml and config/schema/boost.schema.yml.
  $form['htaccess']['boost_apache_etag'] = [
    '#type' => 'radios',
    '#title' => t('ETag Settings'),
    '#default_value' => \Drupal::config('boost.settings')->get('boost_apache_etag'),
    '#options' => [
      3 => "Set FileETag 'MTime Size' - Useful in server clusters (Highly Recommended)",
      2 => "Set FileETag 'All' - Default if enabled",
      1 => "Set FileETag 'None' - Do not send an etag",
      0 => 'Do Nothing',
    ],
    '#description' => t(
      'Uses <a href="!link">FileETag Directive</a> to set <a href="!about">ETags</a> for the files cached by Boost. <a href="!stack">More info on this subject</a>',
      [
        '!link' => Url::fromUri('http://httpd.apache.org/docs/trunk/mod/core.html#fileetag'),
        '!about' => Url::fromUri('http://en.wikipedia.org/wiki/HTTP_ETag'),
        '!stack' => Url::fromUri('http://stackoverflow.com/questions/tagged?tagnames=etag&sort=votes&pagesize=50'),
      ]
    ),
  ];
  // @FIXME
  // Could not extract the default value because it is either indeterminate, or
  // not scalar. You'll need to provide a default value in
  // config/install/boost.settings.yml and config/schema/boost.schema.yml.
  $form['htaccess']['boost_apache_xheader'] = [
    '#type' => 'radios',
    '#title' => t('Boost Tags'),
    '#default_value' => \Drupal::config('boost.settings')->get('boost_apache_xheader'),
    '#options' => [
      1 => 'Set Boost header',
      0 => 'Do not set Boost header',
    ],
    '#description' => t(
      'In order to identify that the page is being served from the cache, Boost can send out a header that will identify any files served from the boost cache.'
    ),
  ];
  // @FIXME
  // Could not extract the default value because it is either indeterminate, or
  // not scalar. You'll need to provide a default value in
  // config/install/boost.settings.yml and config/schema/boost.schema.yml.
  $form['htaccess']['boost_ssl_bypass'] = [
    '#type' => 'checkbox',
    '#title' => t('Bypass the boost cache for ssl requests.'),
    '#default_value' => \Drupal::config('boost.settings')->get('boost_ssl_bypass'),
    '#description' => t('Ticking this is recommended if you use the securepages module.'),
  ];
  // @FIXME
  // Could not extract the default value because it is either indeterminate, or
  // not scalar. You'll need to provide a default value in
  // config/install/boost.settings.yml and config/schema/boost.schema.yml.
  $form['htaccess']['boost_add_default_charset'] = [
    '#type' => 'checkbox',
    '#title' => t('Add "AddDefaultCharset X" to the htaccess rules'),
    '#default_value' => \Drupal::config('boost.settings')->get('boost_add_default_charset'),
    '#description' => t('Depending on your i18n settings you might want this disabled or enabled. X is set below'),
  ];
  // @FIXME
  // Could not extract the default value because it is either indeterminate, or
  // not scalar. You'll need to provide a default value in
  // config/install/boost.settings.yml and config/schema/boost.schema.yml.
  $form['htaccess']['boost_charset_type'] = [
    '#type' => 'textfield',
    '#title' => t('Add "AddDefaultCharset utf-8" to the htaccess rules'),
    '#default_value' => \Drupal::config('boost.settings')->get('boost_charset_type'),
    '#description' => t('Depending on your i18n settings you might want this disabled or enabled.'),
  ];
  // @FIXME
  // Could not extract the default value because it is either indeterminate, or
  // not scalar. You'll need to provide a default value in
  // config/install/boost.settings.yml and config/schema/boost.schema.yml.
  // @FIXME
  // Could not extract the default value because it is either indeterminate, or
  // not scalar. You'll need to provide a default value in
  // config/install/boost.settings.yml and config/schema/boost.schema.yml.
  // @FIXME
  // Could not extract the default value because it is either indeterminate, or
  // not scalar. You'll need to provide a default value in
  // config/install/boost.settings.yml and config/schema/boost.schema.yml.
  // @FIXME
  // Could not extract the default value because it is either indeterminate, or
  // not scalar. You'll need to provide a default value in
  // config/install/boost.settings.yml and config/schema/boost.schema.yml.
  $form['htaccess']['boost_match_symlinks_options'] = [
    '#type' => 'radios',
    '#title' => t('%cache_folder Options', [
      '%cache_folder' =>
        $document_root .
        '/' .
        \Drupal::config('boost.settings')->get('boost_root_cache_dir') .
        '/' .
        \Drupal::config('boost.settings')->get('boost_normal_dir') .
        '/' .
        \Drupal::config('boost.settings')->get('boost_server_name_http_host') .
        '/.htaccess',
    ]),
    '#default_value' => \Drupal::config('boost.settings')->get('boost_match_symlinks_options'),
    '#options' => [
      1 => 'Set "Options +FollowSymLinks"',
      0 => 'Set "Options +SymLinksIfOwnerMatch"',
    ],
    '#description' => t(
      'The .htaccess file in the cache folder requires "Options +FollowSymLinks" or "Options +SymLinksIfOwnerMatch" for mod_rewrite. Some hosting companies only permit the SymLinksIfOwnerMatch option. If you get a http 500 error code try setting SymLinksIfOwnerMatch.'
    ),
  ];

  // reset htaccess on submit;
  $form['#submit'][] = 'boost_form_submit_handler';
  return system_settings_form($form);
}

/**
 * Form builder; Configure boost settings.
 *
 * @ingroup forms
 * @see system_settings_form()
 */
function boost_admin_htaccess_generation() {
  // Generated .htaccess output
  $htaccess = boost_admin_htaccess_generate_htaccess();
  // @FIXME
  // l() expects a Url object, created from a route name or external URI.
  // $form['boost_generated'] = array(
  //     '#type'          => 'textarea',
  //     '#title'         => t('Generated Rules'),
  //     '#default_value' => $htaccess,
  //     '#rows'          => count(explode("\n", $htaccess))+1,
  //     '#wysiwyg'       => FALSE,
  //     '#description'   => t("Copy this into your .htaccess file below <pre><tt>  # If your site is running in a VirtualDocumentRoot at http://example.com/,
  //   # uncomment the following line:
  //   # RewriteBase / </tt></pre> and above <pre><tt>  # Pass all requests not referring directly to files in the filesystem to
  //   # index.php. Clean URLs are handled in drupal_environment_initialize().</tt></pre><br />Note that the generated rules' settings can be configure at !link.", array('!link' => l('admin/config/development/performance/boost/htaccess-settings', 'admin/config/development/performance/boost/htaccess-settings'))),
  //   );

  // reset htaccess on submit;
  $form['#submit'][] = 'boost_form_submit_handler';
  return $form;
}

/**
 * Generate htaccess code.
 *
 * Http://www.askapache.com/htaccess/mod_rewrite-variables-cheatsheet.html
 *
 * @return string
 *   htaccess code
 */
function boost_admin_htaccess_generate_htaccess() {
  global $base_path;
  // @FIXME
  // Could not extract the default value because it is either indeterminate, or
  // not scalar. You'll need to provide a default value in
  // config/install/boost.settings.yml and config/schema/boost.schema.yml.
  $server_name = \Drupal::config('boost.settings')->get('boost_server_name_http_host');
  // @FIXME
  // Could not extract the default value because it is either indeterminate, or
  // not scalar. You'll need to provide a default value in
  // config/install/boost.settings.yml and config/schema/boost.schema.yml.
  $document_root = \Drupal::config('boost.settings')->get('boost_document_root');
  $drupal_subdir = rtrim($base_path, '/');

  // Various dir's
  // @FIXME
  // Could not extract the default value because it is either indeterminate, or
  // not scalar. You'll need to provide a default value in
  // config/install/boost.settings.yml and config/schema/boost.schema.yml.
  $cache_dir = \Drupal::config('boost.settings')->get('boost_root_cache_dir');
  // @FIXME
  // Could not extract the default value because it is either indeterminate, or
  // not scalar. You'll need to provide a default value in
  // config/install/boost.settings.yml and config/schema/boost.schema.yml.
  $normal_dir = \Drupal::config('boost.settings')->get('boost_normal_dir');
  // @FIXME
  // Could not extract the default value because it is either indeterminate, or
  // not scalar. You'll need to provide a default value in
  // config/install/boost.settings.yml and config/schema/boost.schema.yml.
  $char = \Drupal::config('boost.settings')->get('boost_char');

  // Go through every storage type getting the extesion and if it supports gzip.
  $enabled_file_extensions = [];
  $types = boost_get_storage_types();
  foreach ($types as $title => $content_types) {
    foreach ($content_types as $type => $values) {
      if ($values['enabled']) {
        $enabled_file_extensions[$values['extension']]['gzip'] = $values['gzip'];
        if (empty($enabled_file_extensions[$values['extension']]['content_type'])) {
          $enabled_file_extensions[$values['extension']]['content_type'] = $type;
        }
      }
    }
  }

  $output = ['gzip' => '', 'normal' => ''];
  $gzip_count = 0;
  $normal_count = 0;
  foreach ($enabled_file_extensions as $extension => $values) {
    $type = $values['content_type'];
    $output['normal'] .= "  RewriteCond $document_root$base_path$cache_dir/%{ENV:boostpath}/$server_name%{REQUEST_URI}$char%{QUERY_STRING}\.$extension -s\n";
    $output['normal'] .= "  RewriteRule .* $cache_dir/%{ENV:boostpath}/$server_name%{REQUEST_URI}$char%{QUERY_STRING}\.$extension [L,T=$type]\n";
    $normal_count++;
  }
  $skip = !empty($gzip_count) ? $normal_count + $gzip_count + 1 : $normal_count + 1;

  // Generate the rules
  $string = "  ### BOOST START ###\n";

  if (!empty($output)) {
    $string .= "\n";
    $string .= "  # Allow for alt paths to be set via htaccess rules; allows for cached variants (future mobile support)\n";
    $string .= "  RewriteRule .* - [E=boostpath:$normal_dir]\n";
    $string .= "\n";

    $string .= "#  # Apache 2.4 bug workaround\n";
    $string .= "#  # Enables Search from home page https://drupal.org/node/2078595#comment-8724321\n";
    $string .= "#  RewriteCond %{REQUEST_METHOD} ^(POST)$\n";
    $string .= "#  RewriteCond %{REQUEST_URI} $base_path\n";
    $string .= "#  RewriteRule .* $base_path [S=" . ($skip + 1) . "]\n";
    $string .= "\n";

    $string .= "  # Caching for anonymous users\n";
    // @FIXME
    // Could not extract the default value because it is either indeterminate, or
    // not scalar. You'll need to provide a default value in
    // config/install/boost.settings.yml and config/schema/boost.schema.yml.
    $string .=
      '  # Skip boost IF not get request OR uri has wrong dir OR cookie is set OR request came from this server' .
      (\Drupal::config('boost.settings')->get('boost_ssl_bypass') ? ' OR https request' : '') .
      "\n";
    $string .= "  RewriteCond %{REQUEST_METHOD} !^(GET|HEAD)$ [OR]\n";
    $string .= "  RewriteCond %{REQUEST_URI} (^$base_path(admin|$cache_dir|misc|modules|sites|system|openid|themes|node/add|comment/reply))|(/(edit|user|user/(login|password|register))$) [OR]\n";
    // @FIXME
    // Could not extract the default value because it is either indeterminate, or
    // not scalar. You'll need to provide a default value in
    // config/install/boost.settings.yml and config/schema/boost.schema.yml.
    if (\Drupal::config('boost.settings')->get('boost_ssl_bypass')) {
      $string .= "  RewriteCond %{HTTPS} on [OR]\n";
    }
    // @FIXME
    // Could not extract the default value because it is either indeterminate, or
    // not scalar. You'll need to provide a default value in
    // config/install/boost.settings.yml and config/schema/boost.schema.yml.
    $string .= '  RewriteCond %{HTTP_COOKIE} ' . \Drupal::config('boost.settings')->get('boost_cookie') . " [OR]\n";
    $string .= "  RewriteCond %{ENV:REDIRECT_STATUS} 200\n";
    $string .= "  RewriteRule .* - [S=$skip]\n";
    $string .= "\n";

    $string .= "#  # Apache 2.4 bug workaround\n";
    $string .= "#  # Enables caching of index/ home page\n";
    $string .= '#  RewriteCond %{REQUEST_URI} ^' . "$base_path" . "index\.php$\n";
    $string .= "#  RewriteCond $document_root$base_path$cache_dir/%{ENV:boostpath}/%{HTTP_HOST}" . $base_path . '\\' . $char . "%{QUERY_STRING}\.html -s\n";

    $string .= "#  RewriteRule .* $cache_dir/%{ENV:boostpath}/$server_name$base_path" . '\\' . $char . '%{QUERY_STRING}\.html [L,T=text/html]';
    $string .= "\n";
    $string .= "\n";

    $string .= "  # NORMAL\n";
    $string .= $output['normal'];
  }
  $string .= "\n";
  $string .= "  ### BOOST END ###\n";

  return $string;
}

/**
 * Returns all key/values in array that are equal.
 *
 * @param $needle
 *   What your searching for
 * @param $haystack
 *   Array of values
 * @param $a_not
 *   Optional array of key names to exclude
 */
function boost_admin_htaccess_array_find($needle, $haystack, $a_not = []) {
  $out = [];
  foreach ($haystack as $key => $value) {
    if (is_string($value) && strstr($value, $needle) !== FALSE) {
      $good = TRUE;
      foreach ($a_not as $not) {
        if (strpos($key, $not) !== FALSE) {
          $good = FALSE;
        }
      }
      if ($good) {
        $out[$key] = $value;
      }
    }
  }
  return $out;
}
