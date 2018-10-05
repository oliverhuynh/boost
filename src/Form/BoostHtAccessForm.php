<?php

/**
 * @file
 * Contains \Drupal\boost\Form\BoostAdminSettings
 */
namespace Drupal\boost\Form;

use Drupal\Core\Form\ConfigFormBase;
use Drupal\Core\Form\FormStateInterface;

/**
 * Default apache server name.
 */
define('BOOST_SERVER_NAME_HTTP_HOST', '%{HTTP_HOST}');

/**
 * Default apache document root.
 */
define('BOOST_DOCUMENT_ROOT', '%{DOCUMENT_ROOT}');

/**
 * Default setting for SSL pages
 */
define('BOOST_SSL_BYPASS', TRUE);

/**
 * Configure Boost settings for this site.
 */
class BoostHtAccessForm extends ConfigFormBase {
  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'boost_admin_htaccess_settings';
  }

  /**
   * {@inheritdoc}
   */
  protected function getEditableConfigNames() {
    return [
      'boost.settings',
    ];
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    global $base_path;

  // Apache .htaccess settings generation
//   $htaccess = boost_admin_generate_htaccess();
    $form['htaccess'] = array(
      '#type'          => 'fieldset',
      '#title'         => t('Boost Apache .htaccess settings generation'),
      '#description'   => t('<a href="!link">Explanation of .htaccess variables</a> <br /><br />  <em>Be sure to save the configuration and then go to the <a href="!rules">htaccess rules generation page</a> and copy the rules.</em> <br /><strong>Apache 2.4 users should uncomment the two marked sections, each line beginning with #</strong> ', array('!link' => \Drupal\Core\Url::fromUri('http://www.askapache.com/htaccess/mod_rewrite-variables-cheatsheet.html'), '!rules' => \Drupal\Core\Url::fromUri('internal:/admin/config/system/boost/htaccess/generator'))),
    );
    $form['htaccess']['boost_server_name_http_host'] = array(
      '#type'          => 'radios',
      '#title'         => t("Server's URL or Name"),
      '#default_value' => \Drupal::config('boost.settings')->get('boost_server_name_http_host'),
      '#options'       => array(
        '%{HTTP_HOST}' => '%{HTTP_HOST}',
        '%{SERVER_NAME}' => '%{SERVER_NAME}',
        $_SERVER['HTTP_HOST'] => $_SERVER['HTTP_HOST'],
        $_SERVER['SERVER_NAME'] => $_SERVER['SERVER_NAME'],
      ),
      '#description'   => t('Best to leave these as %{}, only try the last option(s) if boost is still not working.'),
    );
    // Set DOCUMENT_ROOT
    $drupal_subdir = rtrim($base_path, '/');
    $document_root = str_replace("\\", '/', getcwd()); // fix windows dir slashes
    $document_root = trim(str_replace($drupal_subdir, '', $document_root)); // remove subdir
    $options = array('%{DOCUMENT_ROOT}' => '%{DOCUMENT_ROOT}', $document_root => $document_root); // initial options
    $rejects = array('SCRIPT_FILENAME', 'DOCUMENT_ROOT'); // values to ignore
    $output = boost_admin_htaccess_array_find($document_root, $_SERVER, $rejects); //search for values that match getcwd
    $description_extra = '';
    if (!empty($output)) {
      foreach ($output as $key => $value) {
        $temp = '%{ENV:' . $key . '}';
        $options[$temp] = $temp . ' = ' . $value; // adding values to options
        if (strcmp($value, $document_root) == 0) {
          $best = $temp; // set best since it's a match
        }
      }
    }
    if (strcmp($_SERVER['DOCUMENT_ROOT'], $document_root) == 0) {
      $best = '%{DOCUMENT_ROOT}';
    }
    elseif (!isset($best)) {
      $best = $document_root;
      $description_extra = t('Please <a href="!link">open an boost issue on Drupal.org</a>, since apache and php might not be configured correctly.', array('!link' => \Drupal\Core\Url::fromUri('http://drupal.org/node/add/project-issue/boost')));
    }
    $percent = 0;
    $int = similar_text(substr(trim($_SERVER['DOCUMENT_ROOT']), 18, 1), substr(trim($document_root), 18, 1), $percent);
    $description = t('Value of %best is recommended for this server.', array('%best' => $best)) . ' ' . $description_extra;
    $form['htaccess']['boost_document_root'] = array(
      '#type'          => 'radios',
      '#title'         => t('Document Root'),
      '#default_value' => \Drupal::config('boost.settings')->get('boost_document_root'),
      '#options'       => $options,
      '#description'   => $description,
    );
    $form['htaccess']['boost_apache_etag'] = array(
      '#type'          => 'radios',
      '#title'         => t('ETag Settings'),
      '#default_value' => \Drupal::config('boost.settings')->get('boost_apache_etag'),
      '#options'       => array(
        3 => "Set FileETag 'MTime Size' - Useful in server clusters (Highly Recommended)",
        2 => "Set FileETag 'All' - Default if enabled",
        1 => "Set FileETag 'None' - Do not send an etag",
        0 => 'Do Nothing',
      ),
      '#description'   => t('Uses <a href="!link">FileETag Directive</a> to set <a href="!about">ETags</a> for the files cached by Boost. <a href="!stack">More info on this subject</a>', array('!link' => \Drupal\Core\Url::fromUri('http://httpd.apache.org/docs/trunk/mod/core.html#fileetag'), '!about' => \Drupal\Core\Url::fromUri('http://en.wikipedia.org/wiki/HTTP_ETag'), '!stack' => \Drupal\Core\Url::fromUri('http://stackoverflow.com/questions/tagged?tagnames=etag&sort=votes&pagesize=50'))),
    );
    $form['htaccess']['boost_apache_xheader'] = array(
      '#type'          => 'radios',
      '#title'         => t('Boost Tags'),
      '#default_value' => \Drupal::config('boost.settings')->get('boost_apache_xheader'),
      '#options'       => array(
        1 => 'Set Boost header',
        0 => 'Do not set Boost header',
      ),
      '#description'   => t('In order to identify that the page is being served from the cache, Boost can send out a header that will identify any files served from the boost cache.'),
    );
    $form['htaccess']['boost_ssl_bypass'] = array(
      '#type'          => 'checkbox',
      '#title'         => t('Bypass the boost cache for ssl requests.'),
      '#default_value' => \Drupal::config('boost.settings')->get('boost_ssl_bypass'),
      '#description'   => t('Ticking this is recommended if you use the securepages module.'),
    );
    $form['htaccess']['boost_add_default_charset'] = array(
      '#type'          => 'checkbox',
      '#title'         => t('Add "AddDefaultCharset X" to the htaccess rules'),
      '#default_value' => \Drupal::config('boost.settings')->get('boost_add_default_charset'),
      '#description'   => t('Depending on your i18n settings you might want this disabled or enabled. X is set below'),
    );
    $form['htaccess']['boost_charset_type'] = array(
      '#type'          => 'textfield',
      '#title'         => t('Add "AddDefaultCharset utf-8" to the htaccess rules'),
      '#default_value' => \Drupal::config('boost.settings')->get('boost_charset_type'),
      '#description'   => t('Depending on your i18n settings you might want this disabled or enabled.'),
    );
    $form['htaccess']['boost_match_symlinks_options'] = array(
      '#type'          => 'radios',
      '#title'         => t('%cache_folder Options', array( '%cache_folder' => $document_root . '/' . \Drupal::config('boost.settings')->get('boost_root_cache_dir') . '/' . \Drupal::config('boost.settings')->get('boost_normal_dir') . '/' . \Drupal::config('boost.settings')->get('boost_server_name_http_host') . '/.htaccess' )),
      '#default_value' => \Drupal::config('boost.settings')->get('boost_match_symlinks_options'),
      '#options'       => array(
        1 => 'Set "Options +FollowSymLinks"',
        0 => 'Set "Options +SymLinksIfOwnerMatch"',
      ),
      '#description'   => t('The .htaccess file in the cache folder requires "Options +FollowSymLinks" or "Options +SymLinksIfOwnerMatch" for mod_rewrite. Some hosting companies only permit the SymLinksIfOwnerMatch option. If you get a http 500 error code try setting SymLinksIfOwnerMatch.'),
    );

    // @TODO: Oliverreset htaccess on submit;
    //$form['#submit'][] = 'boost_form_submit_handler';

    return parent::buildForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    /*$this->config('boost.settings')
      ->set('directory', $form_state->getValue('boost_directory'))
      ->save();*/

    parent::submitForm($form, $form_state);
  }

}

?>
