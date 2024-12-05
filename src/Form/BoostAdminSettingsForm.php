<?php

namespace Drupal\boost\Form;

use Drupal\Core\Form\ConfigFormBase;
use Drupal\Core\Render\Element;
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
    $form['cacheability'] = [
      '#type'          => 'fieldset',
      '#title'         => t('Boost cacheability settings'),
    ];
    // See http://api.drupal.org/api/function/block_admin_configure/7
    $access = \Drupal::currentUser()->hasPermission('use PHP for settings');
    $options = [
      BOOST_VISIBILITY_NOTLISTED => t('All pages except those listed'),
      BOOST_VISIBILITY_LISTED => t('Only the listed pages'),
    ];
    $description = t("Specify pages by using their paths. Enter one path per line. The '*' character is a wildcard. Example paths are %blog for the blog page and %blog-wildcard for every personal blog. %front is the front page.", ['%blog' => 'blog', '%blog-wildcard' => 'blog/*', '%front' => '<front>']);
    if (\Drupal::moduleHandler()->moduleExists('php') && $access) {
      $options += [BOOST_VISIBILITY_PHP => t('Pages on which this PHP code returns <code>TRUE</code> (experts only)')];
      $title = t('Pages or PHP code');
      $description .= ' ' . t('If the PHP option is chosen, enter PHP code between %php. Note that executing incorrect PHP code can break your Drupal site.', ['%php' => '<?php ?>']);
    }
    else {
      $title = t('Pages');
    }
    $form['cacheability']['boost_cacheability_option'] = [
      '#type' => 'radios',
      '#title' => t('Cache specific pages'),
      '#options' => $options,
      '#default_value' => \Drupal::config('boost.settings')->get('boost_cacheability_option'),
    ];
    $form['cacheability']['boost_cacheability_pages'] = [
      '#type' => 'textarea',
      '#title' => '<span class="element-invisible">' . $title . '</span>',
      '#default_value' => \Drupal::config('boost.settings')->get('boost_cacheability_pages'),
      '#description' => $description,
    ];

    $form['extra_clear'] = [
      '#type'          => 'textfield',
      '#title'         => t('Extra clear'),
      '#default_value' => $config->get('extra_clear'),
    ];

    $types = boost_get_storage_types();
    $_tmp = [0, 60, 180, 300, 600, 900, 1800, 2700, 3600, 10800, 21600, 32400, 43200, 64800, 86400, 2 * 86400, 3 * 86400, 4 * 86400, 5 * 86400, 6 * 86400, 604800, 2 * 604800, 3 * 604800, 4 * 604800, 8 * 604800, 16 * 604800, 52 * 604800];
    $period = array_map([\Drupal::service('date.formatter'), 'formatInterval'], array_combine($_tmp, $_tmp));

    $form['cache_types'] = [
      '#type'          => 'fieldset',
      '#title'         => t('Boost cache type settings'),
    ];
    foreach ($types as $title => $content_types) {
      $form['cache_types'][$title] = [
        '#type'          => 'fieldset',
        '#title'         => t('@title settings', ['@title' => $title]),
        '#collapsible' => TRUE,
      ];
      $collapsed = TRUE;
      foreach ($content_types as $type => $values) {
        $form['cache_types'][$title][$type] = [
          '#type'           => 'fieldset',
          '#title'          => t('@type settings', ['@type' => $type]),
          '#description'    => t('Cache @description of type @type',
            [
              '@description' => $values['description'],
              '@type' => $type,
            ]
          ),
        ];
        // This content type enabled?
        $form['cache_types'][$title][$type]['boost_enabled_' . $type] = [
          '#type'          => 'checkbox',
          '#title'         => t('Cache Enabled'),
          '#default_value' => $values['enabled'],
        ];
        // https://drupal.org/node/1416214#comment-7225650
        //      // Enable gzip?
        //      $form['cache_types'][$title][$type]['boost_gzip_' . $type] = array(
        //        '#type'          => 'checkbox',
        //        '#title'         => t('Enable gzip compression'),
        //        '#description'   => (BOOST_GZIP ? t('Avoids having to compress the content by the web server on every request (recommended).') : t('Your host does not support zlib. See: !url', array('!url' => 'http://www.php.net/manual/en/zlib.installation.php'))),
        //        '#default_value' => (BOOST_GZIP ? $values['gzip'] : 0),
        //        '#disabled'      => ! BOOST_GZIP,
        //      );

        // Content type extension
        $form['cache_types'][$title][$type]['boost_extension_' . $type] = [
          '#type'          => 'textfield',
          '#title'         => t('Filename Extension',
            [
              '@title' => $title,
              '@description' => $values['description'],
              '@type' => $type,
            ]
          ),
          '#default_value' => $values['extension'],
        ];

        // Maximum cache lifetime
        $form['cache_types'][$title][$type]['boost_lifetime_max_' . $type] = [
          '#type'          => 'select',
          '#options' => $period,
          '#title'         => t('@type - Maximum Cache Lifetime',
            [
              '@title' => $title,
              '@description' => $values['description'],
              '@type' => $type,
            ]
          ),
          '#default_value' => $values['lifetime_max'],
        ];

        // Minimum cache lifetime
        $form['cache_types'][$title][$type]['boost_lifetime_min_' . $type] = [
          '#type'          => 'select',
          '#options' => $period,
          '#title'         => t('@type - Minimum Cache Lifetime',
            [
              '@title' => $title,
              '@description' => $values['description'],
              '@type' => $type,
            ]
          ),
          '#default_value' => $values['lifetime_min'],
        ];
        if ($values['enabled']) {
          $collapsed = !$values['enabled'];
        }
      }
      $form['cache_types'][$title]['#collapsed'] = $collapsed;
    }

    $form['boost_preload_path'] = [
      '#type'          => 'textfield',
      '#title'         => t('Preload path'),
      '#default_value' => $config->get('boost_preload_path'),
      '#description' => t('The path to enable preload backend service. Change to /tmp to disable the preload background service.'),
    ];

    $form['cache_404'] = [
        '#type'          => 'checkbox',
        '#title'         => t('Cache 404 pages'),
        '#default_value' => $config->get('cache_404'),
        '#description' => t('To reduce the load to the database by various of crawl, bots, just turn this on.'),
    ];

    // @TODO: Oliver
    // $form['#submit'][] = 'boost_form_submit_handler';
    $form['#config']['location'] = $this->getEditableConfigNames()[0];
    $form['#config']['keys'] = $this->mychildren($form);

    return parent::buildForm($form, $form_state);
  }

  public function mychildren($form) {
    $ret = Element::children($form);
    $children = [];
    foreach ($ret as $t => $key) {
      if ($form[$key]['#type'] == 'fieldset') {
        unset($ret[$t]);
        $children = array_merge($children, $this->mychildren($form[$key]));
      }
    }
    $ret = array_merge($ret, $children);
    return $ret;
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    foreach ($form['#config']['keys'] as $key) {
      $this->config($form['#config']['location'])
        ->set($key, $form_state->getValue($key));
    }

    $this->config($form['#config']['location'])->save();
    parent::submitForm($form, $form_state);
  }

}
