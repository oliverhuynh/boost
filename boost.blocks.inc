<?php

/**
 * @file
 * Prints the cache status of the currently displayed page.
 *
 * See @boost_block_view()
 */

function boost_block_view_status() {
  $user = \Drupal::currentUser();

  $block = [];
  $block['subject'] = '';

  // Don't show the block to anonymous users
  if (!$user->uid) {
    return $block;
  }

  // Do not use the global $_boost to not confuse hook_exit()
  $_boost = boost_transform_url();

  // Unset these variables otherwise boost_is_cacheable() will quickly bail out.
  unset($_boost['is_cacheable']);
  unset($_boost['is_cacheable_reason']);

  $_boost = boost_is_cacheable($_boost, 'status');

  if (!$_boost['is_cacheable']) {
    $reason = $_boost['is_cacheable_reason'] ? $_boost['is_cacheable_reason'] : 'reason unknown';
    $block['content']['is_not_cacheable'] = [
      '#markup' => '<p>' . t('This page will not be cached: %reason', ['%reason' => $reason]) . '</p>',
    ];

    return $block;
  }

  // We need the extention for the filename
  $_boost['header_info'] = boost_get_header_info();
  $_boost['matched_header_info'] = boost_match_header_attributes($_boost['header_info']);

  $filename = isset($_boost['filename']) ? $_boost['filename'] . '.' . $_boost['matched_header_info']['extension'] : 'n/a';

  if (file_exists($filename)) {
    // be precise on the time (seconds and timezone)
    $generated = date('c', filemtime($filename));
  }
  else {
    $generated = 'not cached yet (either no one has visited the page recently, or something is preventing the cache from being generated).';
  }

  $block['content'] = [
    'filename' => [
      '#markup' => '<p>' . t('File: %filename', ['%filename' => $filename]) . '</p>',
    ],
    'generated' => [
      '#markup' => '<p>' . t('Generated: %generated', ['%generated' => $generated]) . '</p>',
    ],
  ];

  if (file_exists($filename) && \Drupal::currentUser()->hasPermission('boost flush pages')) {
    // 1922532 - Variable required to remove warning under PHP 5.4
    $form = \Drupal::formBuilder()->getForm('boost_block_flush_form');
    $block['content']['flush'] = [
      '#markup' => \Drupal::service('renderer')->render($form),
    ];
  }

  return $block;
}

function boost_block_flush_form() {
  $form = [];

  $form['boost_cache']['clear'] = [
    '#type' => 'submit',
    '#value' => t('Flush Page'),
  ];

  return $form;
}

function boost_block_flush_form_submit() {
  $_boost = boost_transform_url();

  // We need the extention for the filename
  $_boost['header_info'] = boost_get_header_info();
  $_boost['matched_header_info'] = boost_match_header_attributes($_boost['header_info']);

  $filename = $_boost['filename'] . '.' . $_boost['matched_header_info']['extension'];

  if (is_file($filename)) {
    if (unlink($filename)) {
      \Drupal::messenger()->addStatus(t('%filename was deleted from the Boost cache', ['%filename' => $filename]));
    }
    else {
      \Drupal::messenger()->addStatus(
        t('%filename could not be deleted, check file permissions on disk to see if the web server can write/delete the file.', ['%filename' => $filename])
          );
    }
  }
  else {
    \Drupal::messenger()->addStatus(t('%filename was not found in the Boost cache and could not be deleted.', ['%filename' => $filename]));
  }
}
