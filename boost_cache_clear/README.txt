Boost cache clear (BCC) is Supporting module for Boost module.

It clear specific Boost cache (delete particular static boost cache
pages) when Node create, Node edit/update, Node delete,
Node comment create, Node comment edit/update and Node comment delete.

Boost module cache the pages as static HTML, When new node add/edit/delete
the listing of pages won't changes. It only affects by flush all cache or
the page get expire. Boost cache clear module clear the specific page cache.

Configuration:
--------------

It provide a interface "Boost Cache Clear Settings" on all Content page.
There we can specify the which all pages we want to clear Boost cache
if Node add/edit/delete and Comment add/edit/delete. So we can improve
our system with high performance.

Note: To clear boost cache Specify pages by using their paths. Enter one
path per line. The "*" character is a wild card. Example paths are blog
for the blog page and blog/* for every personal blog and blog?page=* for
 query strings. / is the front page.

Configuration Steps(CCK Level):
-------------------------------
Step 1: Enable this module(admin/modules)
Step 2: Enable Boost cache clear for content types
      (admin/structure/types/manage/CONTENT_TYPE_MACHINE_NAME)
      Example : admin/structure/types/manage/article
Step 3: Click "Boost cache Clear settings" tab on Right side tabs.
Step 4: Enable checkbox for clear boost cache of node to this content type.
Step 5: Additionally add the Drupal paths needs to clear cache when node
        created or updated for this content type.
        
Clear Individual Page Cache:
----------------------------
This module provide the User Interface to clear specific page cache
independent any Drupal core events.

SITE_URL/admin/config/system/boost-cache-clear

Boost Cache Cleaner does not work for anonymous user. Specify the Roles who can
access the Boost Cache clear pages by giving the permission "Boost Cache Clear".

SITE_URL/admin/people/permissions 

Dependencies - boost(https://www.drupal.org/project/boost)

Below are the core events when the boost cache clear flush
(delete the static pages) the cache.
1. Node insert, update and delete.
2. Comment insert, update and delete.

Module is extend to your custom module by calling the function
boost_cache_clear_by_url()

Syntax:

/**
  * @param $url valid drupal path.
  */
boost_cache_clear_by_url($url);

Note: The "*" character is a wild card. Example paths are blog for the blog page
and blog/* for every personal blog. / is the front page.
