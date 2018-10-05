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
function boost_admin_debug_settings() {

  // @FIXME
// Could not extract the default value because it is either indeterminate, or
// not scalar. You'll need to provide a default value in
// config/install/boost.settings.yml and config/schema/boost.schema.yml.
$form['boost_message_debug'] = array(
    '#type' => 'checkbox',
    '#title' => t('Send debug info for each request to watchdog.'),
    '#default_value' => \Drupal::config('boost.settings')->get('boost_message_debug'),
    '#description' => t('Only use for debugging purposes as this can fill up watchdog fairly quickly.'),
  );

  // reset htaccess on submit;
  $form['#submit'][] = 'boost_form_submit_handler';
  return system_settings_form($form);
}
