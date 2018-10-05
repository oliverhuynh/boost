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
function boost_admin_expiration_settings() {

  // @FIXME
// url() expects a route name or an external URI.
// $form['boost_ignore_flush'] = array(
//     '#type' => 'checkbox',
//     '#title' => t('Ignore a cache flush command if cron issued the request.'),
//     '#default_value' => variable_get('boost_ignore_flush', BOOST_IGNORE_FLUSH),
//     '#description' => t('Drupal will flush all caches when cron is executed, depending on the <a href="!urlcore">core minimum cache lifetime</a> setting. To ignore the request to flush the cache on cron runs, enable this option. If enabled, pages in the Boost cache will be flushed only when their <a href="!urlboost">Boost maximum cache lifetime</a> expires.',
//       array(
//         '!urlcore'  => \Drupal\Core\Url::fromRoute('system.performance_settings'),
//         '!urlboost' => url('admin/config/system/boost'),
//       )),
//   );

  // @FIXME
// Could not extract the default value because it is either indeterminate, or
// not scalar. You'll need to provide a default value in
// config/install/boost.settings.yml and config/schema/boost.schema.yml.
$form['boost_expire_cron'] = array(
    '#type' => 'checkbox',
    '#title' => t('Remove old cache files on cron.'),
    '#default_value' => \Drupal::config('boost.settings')->get('boost_expire_cron'),
    '#description' => t('If enabled, each time cron runs Boost will check each cached page and delete those that have expired (maximum cache lifetime). The expiration time is displayed in the comment that Boost adds to the bottom of the html pages it creates. This setting is recommended for most sites.'),
  );

  // reset htaccess on submit;
  $form['#submit'][] = 'boost_form_submit_handler';
  return system_settings_form($form);
}
