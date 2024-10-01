<?php

namespace Drupal\boost_cache_clear\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;

class BoostCacheClearSettingsForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'boost_cache_clear_settings_form';
  }

  public function buildForm(array $form, FormStateInterface $form_state) {
    $form['path'] = [
      '#type' => 'textfield',
      '#size' => 120,
      '#maxlength' => 250,
      '#required' => TRUE,
      '#title' => t('Clear Boost'),
      '#description' => t('Enter valid URL alias for clear Boost cache. The "*" character is a wildcard. Example paths are blog for the blog page and blog/* for every personal blog and blog?page=* for query strings. / is the front page. "*" is for clear entire boost cache'),
    ];
    $form['submit'] = [
      '#type' => 'submit',
      '#value' => t('Clear'),
    ];
    return $form;
  }

  public function validateForm(array &$form, FormStateInterface $form_state) {
    $path = $form_state->getValue(['path']);
    if (!boost_cache_clear_valid_url($path)) {
      $form_state->setErrorByName('path', t('Please enter valid URL.'));
    }
  }

  public function submitForm(array &$form, FormStateInterface $form_state) {
    $path = $form_state->getValue(['path']);
    boost_cache_clear_by_url($path);
    $this->messenger()->addStatus(t('Boost cache cleared successfully!'));
  }

}
