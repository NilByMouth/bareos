/*
   BAREOS® - Backup Archiving REcovery Open Sourced

   Copyright (C) 2004-2008 Free Software Foundation Europe e.V.

   This program is Free Software; you can redistribute it and/or
   modify it under the terms of version three of the GNU Affero General Public
   License as published by the Free Software Foundation and included
   in the file LICENSE.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Affero General Public License for more details.

   You should have received a copy of the GNU Affero General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.
*/
/*
 * BAREOS Director -- User Agent Access Control List (ACL) handling
 *
 * Kern Sibbald, January MMIV
 */

#include "bareos.h"
#include "dird.h"

/*
 * Check if access is permitted to item in acl
 */
bool acl_access_ok(UAContext *ua, int acl, const char *item)
{
   return acl_access_ok(ua, acl, item, strlen(item));
}


/* This version expects the length of the item which we must check. */
bool acl_access_ok(UAContext *ua, int acl, const char *item, int len)
{
   /* The resource name contains nasty characters */
   if (acl != Where_ACL && acl != PluginOptions_ACL && !is_name_valid(item, NULL)) {
      Dmsg1(1400, "Access denied for item=%s\n", item);
      return false;
   }

   /* If no console resource => default console and all is permitted */
   if (!ua->cons) {
      Dmsg0(1400, "Root cons access OK.\n");
      return true;                    /* No cons resource -> root console OK for everything */
   }

   alist *list = ua->cons->ACL_lists[acl];
   if (!list) {                       /* empty list */
      if (len == 0 && acl == Where_ACL) {
         return true;                 /* Empty list for Where => empty where */
      }
      return false;                   /* List empty, reject everything */
   }

   /* Special case *all* gives full access */
   if (list->size() == 1 && bstrcasecmp("*all*", (char *)list->get(0))) {
      return true;
   }

   /* Search list for item */
   for (int i=0; i<list->size(); i++) {
      if (bstrcasecmp(item, (char *)list->get(i))) {
         Dmsg3(1400, "ACL found %s in %d %s\n", item, acl, (char *)list->get(i));
         return true;
      }
   }
   return false;
}
