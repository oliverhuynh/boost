<?php

/**
 * @file
 * Admin page callbacks for the boost module.
 */

/**
 * Form builder; Configure boost settings.
 *
 * @ingroup forms
 * @see system_settings_form()
 */
function boost_admin_filesystem_settings() {

  // @FIXME
// Could not extract the default value because it is either indeterminate, or
// not scalar. You'll need to provide a default value in
// config/install/boost.settings.yml and config/schema/boost.schema.yml.
$form['boost_root_cache_dir'] = array(
    '#type' => 'textfield',
    '#title' => t('Root cache directory'),
    '#default_value' => \Drupal::config('boost.settings')->get('boost_root_cache_dir'),
  );
  // @FIXME
// Could not extract the default value because it is either indeterminate, or
// not scalar. You'll need to provide a default value in
// config/install/boost.settings.yml and config/schema/boost.schema.yml.
$form['boost_normal_dir'] = array(
    '#type' => 'textfield',
    '#title' => t('Normal cache directory'),
    '#default_value' => \Drupal::config('boost.settings')->get('boost_normal_dir'),
  );
  // @FIXME
// Could not extract the default value because it is either indeterminate, or
// not scalar. You'll need to provide a default value in
// config/install/boost.settings.yml and config/schema/boost.schema.yml.
$form['boost_char'] = array(
    '#type' => 'textfield',
    '#title' => t('Character replacement for "?" in the URL'),
    '#default_value' => \Drupal::config('boost.settings')->get('boost_char'),
  );

  // reset htaccess on submit;
  $form['#submit'][] = 'boost_form_submit_handler';
  return system_settings_form($form);
}
