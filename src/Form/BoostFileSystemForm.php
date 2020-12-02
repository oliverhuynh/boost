<?php

/**
 * @file
 * Contains \Drupal\boost\Form\BoostAdminSettings
 */
namespace Drupal\boost\Form;

use Drupal\Core\Form\ConfigFormBase;
use Drupal\Core\Form\FormStateInterface;

/**
 * Configure Boost settings for this site.
 */
class BoostFileSystemForm extends ConfigFormBase {
  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'boost_admin_filesystem_settings';
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
    $form['boost_root_cache_dir'] = array(
      '#type' => 'textfield',
      '#title' => t('Root cache directory'),
      '#default_value' => \Drupal::config('boost.settings')->get('boost_root_cache_dir'),
    );
    $form['boost_normal_dir'] = array(
      '#type' => 'textfield',
      '#title' => t('Normal cache directory'),
      '#default_value' => \Drupal::config('boost.settings')->get('boost_normal_dir'),
    );
    $form['boost_char'] = array(
      '#type' => 'textfield',
      '#title' => t('Character replacement for "?" in the URL'),
      '#default_value' => \Drupal::config('boost.settings')->get('boost_char'),
    );

    // @TODO: Oliver: reset htaccess on submit;
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

?>
