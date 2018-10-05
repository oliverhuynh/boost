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
class BoostAdminSettingsForm extends ConfigFormBase {
  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'boost_admin_settings';
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
    $config = $this->config('boost.settings');

    $default = $config->get('directory');
    $default_directory = isset($default) ? $default : 'cache';
    $form['boost_directory'] = array(
      '#type' => 'textfield',
      '#title' => $this->t('Cache directory'),
      '#default_value' => $default_directory,
      '#description' => 'Enter the directory name, relative to the Drupal root, to store cached files.'
    );

    $form['htaccess'] = array(
      '#markup' => '<p>Add the following rules to your .htaccess file in the Drupal root, directly after
      <em>  RewriteRule ^core/install.php core/install.php?rewrite=ok [QSA,L]</em>:</p>
      <pre>
      ### CACHE START ###
      # Caching for anonymous users
      # Skip boost IF not get request OR uri has wrong dir OR cookie is set OR request came from this server OR https request
      RewriteCond %{REQUEST_METHOD} !^(GET|HEAD)$ [OR]
      RewriteCond %{REQUEST_URI} (^/(admin|' . $default_directory . '|misc|modules|sites|system|openid|themes|node/add|comment/reply))|(/(edit|user|user/(login|password|register))$) [OR]
      RewriteCond %{HTTPS} on [OR]
      RewriteCond %{HTTP_COOKIE} DRUPAL_UID [OR]
      RewriteCond %{ENV:REDIRECT_STATUS} 200
      RewriteRule .* - [S=1]#
      RewriteCond %{DOCUMENT_ROOT}/' . $default_directory . '/%{REQUEST_URI}.json -s
      RewriteRule .* ' . $default_directory . '/%{REQUEST_URI}.json [L,T=text/javascript]
      ### CACHE END ###
      </pre>',
    );

    $form['actions']['clear'] = array(
      '#type' => 'submit',
      '#value' => 'Clear the file cache',
      '#access' => TRUE,
      '#submit' => array('_boost_clear_submit'),
      '#name' => 'op',
      '#weight' => 10,
    );

    return parent::buildForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $this->config('boost.settings')
      ->set('directory', $form_state->getValue('boost_directory'))
      ->save();

    parent::submitForm($form, $form_state);
  }

}

?>
