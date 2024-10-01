<?php

/**
 * @file
 * Define Administrator form to configure cache management.
 */
/**
 * Validate the Boost clear URLs.
 */
function boost_cache_clear_confirm_validate($form, &$form_state) {
  $url_array = explode(PHP_EOL, $form_state['values']['page_clear_bcc']);
  foreach ($url_array as $urls) {
    if (!boost_cache_clear_valid_url($urls)) {
      form_set_error('page_clear_bcc', t('Please enter valid URLs.'));
    }
  }
}
/**
 * Submit callback to add/update path in boost cache table.
 */
function boost_cache_clear_confirm_submit($form, &$form_state) {
  $data = array();
  $enable_bcc = empty($form_state['values']['enable_bcc']) ? 0 : 1;
  $data['content_type'] = !empty($form_state['values']['type']) ? $form_state['values']['type'] : '';
  $already_exist = boost_cache_clear_query_handle('count', $data);
  if ($enable_bcc) {
    $data['pages'] = trim($form_state['values']['page_clear_bcc']);
    if (!$already_exist) {
      // Insert if already not exist.
      boost_cache_clear_query_handle('insert', $data);
    }
    else {
      // Update if already exist.
      boost_cache_clear_query_handle('update', $data);
    }
  }
  elseif (!$enable_bcc && $already_exist) {
    // Delete the data if disable.
    boost_cache_clear_query_handle('delete', $data);
  }
}
/**
 * Submit callback to add/update path in boost cache table.
 */
function boost_cache_clear_confirm_delete($form, &$form_state) {
  $data['content_type'] = !empty($form_state['values']['type']) ? $form_state['values']['type'] : '';
  $already_exist = boost_cache_clear_query_handle('count', $data);
  if ($already_exist) {
    boost_cache_clear_query_handle('delete', $data);
  }
}
/**
 * Custom function to Data manupulate on boost_cache_clear table.
 */
function boost_cache_clear_query_handle($type, $data) {
  $return = '';
  switch ($type) {
    case 'insert':
      // TODO: Drupal Rector Notice: Please delete the following comment after you've made any necessary changes.
      // You will need to use `\Drupal\core\Database\Database::getConnection()` if you do not yet have access to the container here.
      $return = \Drupal::database()->insert('boost_cache_clear')
        ->fields(array(
          'id' => NULL,
          'content_type' => $data['content_type'],
          'pages' => $data['pages'],
        ))
        ->execute();
      break;

    case 'update':
      // TODO: Drupal Rector Notice: Please delete the following comment after you've made any necessary changes.
      // You will need to use `\Drupal\core\Database\Database::getConnection()` if you do not yet have access to the container here.
      $return = \Drupal::database()->update('boost_cache_clear')
        ->fields(array(
          'pages' => $data['pages'],
        ))
        ->condition('content_type', $data['content_type'])
        ->execute();
      break;

    case 'delete':
      // TODO: Drupal Rector Notice: Please delete the following comment after you've made any necessary changes.
      // You will need to use `\Drupal\core\Database\Database::getConnection()` if you do not yet have access to the container here.
      $return = \Drupal::database()->delete('boost_cache_clear')
        ->condition('content_type', $data['content_type'])
        ->execute();
      break;

    case 'select':
      // TODO: Drupal Rector Notice: Please delete the following comment after you've made any necessary changes.
      // You will need to use `\Drupal\core\Database\Database::getConnection()` if you do not yet have access to the container here.
      $return = \Drupal::database()->select('boost_cache_clear', 'bcc')
        ->fields('bcc', array('pages'))
        ->condition('content_type', $data['content_type'])
        ->execute()
        ->fetchField();
      break;

    case 'count':
      // TODO: Drupal Rector Notice: Please delete the following comment after you've made any necessary changes.
      // You will need to use `\Drupal\core\Database\Database::getConnection()` if you do not yet have access to the container here.
      $query = \Drupal::database()->select('boost_cache_clear', 'bcc')
        ->condition('content_type', $data['content_type']);
      $query->addExpression('COUNT(bcc.pages)');
      $return = $query->execute()->fetchField();
      break;

    default:
      break;
  }
  return $return;
}
/**
 * Form constructor for the Boost Cache Clear form.
 *
 * @see boost_cache_clear_settings_form_validate()
 * @see boost_cache_clear_settings_form_submit()
 */
function boost_cache_clear_settings_form() {
  $form['path'] = array(
    '#type' => 'textfield',
    '#size' => 120,
    '#maxlength' => 250,
    '#required' => TRUE,
    '#title' => t('Clear Boost'),
    '#description' => t('Enter valid URL alias for clear Boost cache. The "*" character is a wildcard. Example paths are blog for the blog page and blog/* for every personal blog and blog?page=* for query strings. / is the front page. "*" is for clear entire boost cache'),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Clear'),
  );
  return $form;
}
/**
 * Form validation handler for boost_cache_clear_settings_form().
 *
 * @see boost_cache_clear_settings_form_submit()
 */
function boost_cache_clear_settings_form_validate(&$form, &$form_state) {
  $path = $form_state['values']['path'];
  if (!boost_cache_clear_valid_url($path)) {
    form_set_error('path', t('Please enter valid URL.'));
  }
}
/**
 * Form submission handler for boost_cache_clear_settings_form().
 *
 * @see boost_cache_clear_settings_form_validate()
 */
function boost_cache_clear_settings_form_submit(&$form, &$form_state) {
  $path = $form_state['values']['path'];
  boost_cache_clear_by_url($path);
  \Drupal::messenger()->addStatus(t('Boost cache cleared successfully!'));
}
/**
 * Custom function for Validate URLs.
 */
function boost_cache_clear_valid_url($url) {
  $path = explode("?", trim($url));
  $return = 1;
  if (substr($path[0], -1) == "*") {
    $return = 1;
  }
  elseif (!\Drupal::service("path.validator")->isValid($path[0])) {
    if (!drupal_lookup_path('source', $path[0])) {
      $return = 0;
    }
  }
  unset($path);
  return $return;
}
