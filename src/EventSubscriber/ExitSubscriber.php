<?php

namespace Drupal\boost\EventSubscriber;

use Symfony\Component\HttpKernel\KernelEvents;
use Symfony\Component\EventDispatcher\EventSubscriberInterface;
use Drupal\Core\Render\HtmlResponse;
use Drupal\Core\Render\AttachmentsResponseProcessorInterface;
use Drupal\Core\Session\AccountProxy;
use Symfony\Component\HttpKernel\Event\FilterResponseEvent;

class ExitSubscriber implements EventSubscriberInterface {
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
    return [
      KernelEvents::RESPONSE => [
        ['onEvent', -100],
      ],
      // KernelEvents::TERMINATE => ['onEvent', -100]
    ];
  }

  public function onEvent(FilterResponseEvent $event) {
    global $_boost;
    // Bypass caching on redirects (issues #1176534 and #1957532).
    if (\Drupal::currentUser()->id()) {
      return;
    }

    $response = $event->getResponse();
    if (!$response instanceof HtmlResponse) {
      return;
    }

    if ($response->isRedirect() || $response->isForbidden() ||
        $response->isNotFound()) {
      return;
    }

    // Bail out of caching.
    if (!isset($_boost['cache_this'])) {
      if (!isset($_boost['is_cacheable'])) {
        return;
      }
      elseif (!$_boost['is_cacheable']) {
        return;
      }
    }

    // Do not cache 404
    if ($_boost['menu_item']['status'] == 404) {
      $cache_404 = \Drupal::config('boost.settings')->get('cache_404');
      if ($cache_404) {
        $_boost['force'] = TRUE;
      }
    }

    if (!isset($_boost['force'])) {
      if (isset($_boost['cache_this']) && $_boost['cache_this'] == FALSE) {
        return;
      }
      elseif (!isset($_boost['is_cacheable']) || !$_boost['is_cacheable']) {
        return;
      }
      elseif ($_boost['menu_item']['status'] != 200) {
        return;
      }
    }
    /*
    @TODO: Drupal8 way
    elseif (!drupal_page_is_cacheable()) {
    $_boost['is_cacheable'] = FALSE;
    return;
    }*/

    // Get the data to cache.
    $this->htmlResponseAttachmentsProcessor->processAttachments($response);
    $data = $response->getContent();

    // Get header info.
    $_boost['header_info'] = boost_get_header_info();
    $_boost['matched_header_info'] = boost_match_header_attributes($_boost['header_info']);
    if ($_boost['matched_header_info']['enabled'] === FALSE) {
      return;
    }

    // Ensure HTML extension to contains correct HTML tag since strange bug
    // under home_.html
    if ($_boost['matched_header_info']['extension'] == 'html' && (stripos($data, '<html') === FALSE)) {
      // TODO: Log
      // print "Data to cache is not valid";
      // die(0);
      return;
    }

    // Add note to bottom of content if possible.
    if ($_boost['matched_header_info']['comment_start'] && $_boost['matched_header_info']['comment_end']) {
      $expire = $_boost['matched_header_info']['lifetime_max'];
      $cached_at = date('Y-m-d H:i:s', \Drupal::time()->getRequestTime());
      $expires_at = date('Y-m-d H:i:s', \Drupal::time()->getRequestTime() + $expire);
      $note = "\n" . $_boost['matched_header_info']['comment_start'] . 'Page cached by Boost @ ' . $cached_at . ', expires @ ' . $expires_at . ', lifetime ' . \Drupal::service("date.formatter")->formatInterval($expire) . $_boost['matched_header_info']['comment_end'];
      $data .= $note;
    }

    // Write data to a file.
    if (!isset($_boost['filenames'])) {
      $_boost['filename'] .= '.' . $_boost['matched_header_info']['extension'];
      // Write to file.
      $_boost['filename'] = str_ireplace('%2F', '/', $_boost['filename']);
      $_boost['filenames'] = [[$_boost['filename'], $_boost['directory']]];
    }

    foreach ($_boost['filenames'] as $f) {
      $f += [
        FALSE,
        FALSE,
        (isset($_boost['cachestatic']) && $_boost['cachestatic']),
      ];
      list($filename, $directory, $cachestatic) = $f;
      // Attach extension to filename.
      if ($cachestatic) {
        // Change location of cache to cache static
        $data = str_replace('cache/', 'cache/static/', $data);
        $filename = str_replace('cache/', 'cache/static/', $filename);
      }
      boost_write_file($filename, $data);

      // Gzip support.
      if (BOOST_GZIP && $_boost['matched_header_info']['gzip']) {
        // #1416214 https://drupal.org/node/1416214#comment-7225650
        // boost_write_file($_boost['filename'] . '.gz', gzencode($data, 9));
      }
    }
  }

}
