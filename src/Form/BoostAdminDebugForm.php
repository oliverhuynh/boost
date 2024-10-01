<?php

namespace Drupal\boost\Form;

use Drupal\Core\Form\ConfigFormBase;
use Drupal\Core\Form\FormStateInterface;

/**
 * Configure Boost settings for this site.
 */
class BoostAdminDebugForm extends ConfigFormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'boost_admin_debug_settings';
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
    $form['boost_message_debug'] = [
      '#type' => 'checkbox',
      '#title' => t('Send debug info for each request to watchdog.'),
      '#default_value' => \Drupal::config('boost.settings')->get('boost_message_debug'),
      '#description' => t('Only use for debugging purposes as this can fill up watchdog fairly quickly.'),
    ];

    // reset htaccess on submit;
    // @TODO: Oliver
    // $form['#submit'][] = 'boost_form_submit_handler';

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
