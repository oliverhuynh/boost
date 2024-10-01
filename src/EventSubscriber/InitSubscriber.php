<?php

namespace Drupal\boost\EventSubscriber;

use Symfony\Component\HttpKernel\KernelEvents;
use Symfony\Component\EventDispatcher\EventSubscriberInterface;
use Drupal\Core\Render\AttachmentsResponseProcessorInterface;
use Drupal\Core\Session\AccountProxy;

class InitSubscriber implements EventSubscriberInterface {
  protected $htmlResponseAttachmentsProcessor;
  protected $account;

  /**
   * Constructs a HtmlResponseSubscriber object.
   *
   * @param \Drupal\Core\Render\AttachmentsResponseProcessorInterface $html_response_attachments_processor
   *   The HTML response attachments processor service.
   * @param \Drupal\Core\Session\AccountProxy $account
   */
  public function __construct(AttachmentsResponseProcessorInterface $html_response_attachments_processor, AccountProxy $account) {
    $this->htmlResponseAttachmentsProcessor = $html_response_attachments_processor;
    $this->account = $account;
  }

  /**
   * {@inheritdoc}
   */
  public static function getSubscribedEvents() {
    return [KernelEvents::REQUEST => ['onEvent', 101]];
  }

  public function onEvent() {
    global $_boost;

    $_boost = [];

    // Make sure the page is/should be cached according to our current configuration.
    // Start with the quick checks
    // do not cache pages with messages
    if (strpos($_SERVER['SCRIPT_FILENAME'], 'index.php') === FALSE || $_SERVER['SERVER_SOFTWARE'] === 'PHP CLI' || ($_SERVER['REQUEST_METHOD'] != 'GET' && $_SERVER['REQUEST_METHOD'] != 'HEAD') || isset($_GET['nocache']) || \Drupal::config('system')->get('maintenance_mode') || defined('MAINTENANCE_MODE') || !empty($_SESSION['messages'])
    ) {
      $_boost['cache_this'] = FALSE;
    }
    else {
      // More advanced checks
      $_boost = boost_transform_url();
      if (empty($_boost['menu_item']['status']) || $_boost['menu_item']['status'] != 200) {
        $_boost['cache_this'] = FALSE;
      }
    }

    // Give modules a chance to alter the cookie handler callback used.
    // hook_boost_cookie_handler_callback_alter
    $cookie_handler_callback = 'boost_cookie_handler';
    \Drupal::moduleHandler()->alter('boost_cookie_handler_callback', $cookie_handler_callback);
    if (function_exists($cookie_handler_callback)) {
      $cookie_handler_callback();
    }
  }

}
