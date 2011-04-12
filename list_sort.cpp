/*
** Taiga, a lightweight client for MyAnimeList
** Copyright (C) 2010-2011, Eren Okka
** 
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "std.h"
#include "animelist.h"
#include "common.h"
#include "string.h"
#include "win32/win_control.h"

// =============================================================================

int CALLBACK ListViewCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
  if (!lParamSort) return 0;
  CListView* m_List = reinterpret_cast<CListView*>(lParamSort);
  int return_value = 0;

  switch (m_List->GetSortType()) {
    // Number
    case LISTSORTTYPE_NUMBER: {
      WCHAR szItem1[MAX_PATH], szItem2[MAX_PATH];
      m_List->GetItemText(lParam1, m_List->GetSortColumn(), szItem1);
      m_List->GetItemText(lParam2, m_List->GetSortColumn(), szItem2);
      int iItem1 = _wtoi(szItem1);
      int iItem2 = _wtoi(szItem2);
      if (iItem1 > iItem2) {
        return_value = 1;
      } else if (iItem1 < iItem2) {
        return_value = -1;
      }
      break;
    }
    
    // Season
    case LISTSORTTYPE_SEASON: {
      CAnime* pItem1 = reinterpret_cast<CAnime*>(m_List->GetItemParam(lParam1));
      CAnime* pItem2 = reinterpret_cast<CAnime*>(m_List->GetItemParam(lParam2));
      if (pItem1 && pItem2) {
        wstring date1, date2;
        date1 = pItem1->Series_Start;
        date2 = pItem2->Series_Start;
        // if (date1[0] == '0') date1[0] = '?';
        // if (date2[0] == '0') date2[0] = '?';
        if (date1[5] == '0' && date1[6] == '0') date1[5] = '?';
        if (date2[5] == '0' && date2[6] == '0') date2[5] = '?';
        if (date1[8] == '0' && date1[9] == '0') date1[8] = '?';
        if (date2[8] == '0' && date2[9] == '0') date2[8] = '?';
        return_value = wcsncmp(date1.c_str(), date2.c_str(),10);
      }
      break;
    }

    // File size
    case LISTSORTTYPE_FILESIZE: {
      wstring item[2], unit[2];
      UINT64 size[2] = {1, 1};
      m_List->GetItemText(lParam1, m_List->GetSortColumn(), item[0]);
      m_List->GetItemText(lParam2, m_List->GetSortColumn(), item[1]);
      for (size_t i = 0; i < 2; i++) {
        TrimRight(item[i], L".\r");
        EraseChars(item[i], L" ");
        if (item[i].length() >= 2) {
          unit[i] = item[i].substr(item[i].length() - 2);
          item[i].resize(item[i].length() - 2);
        }
        int index = InStr(item[i], L".");
        if (index > -1) {
          int length = item[i].substr(index + 1).length();
          if (length <= 2) item[i].append(2 - length, '0');
          EraseChars(item[i], L".");
        } else {
          item[i].append(2, '0');
        }
        if (IsEqual(unit[i], L"KB")) {
          size[i] *= 1000;
        } else if (IsEqual(unit[i], L"MB")) {
          size[i] *= 1000 * 1000;
        } else if (IsEqual(unit[i], L"GB")) {
          size[i] *= 1000 * 1000 * 1000;
        }
        size[i] *= _wtoi(item[i].c_str());
      }
      if (size[0] > size[1]) {
        return_value = 1;
      } else if (size[0] < size[1]) {
        return_value = -1;
      }
      break;
    }
    
    // Text
    case LISTSORTTYPE_DEFAULT:
    default:
      WCHAR szItem1[MAX_PATH], szItem2[MAX_PATH];
      m_List->GetItemText(lParam1, m_List->GetSortColumn(), szItem1);
      m_List->GetItemText(lParam2, m_List->GetSortColumn(), szItem2);
      return_value = lstrcmpi(szItem1, szItem2);
  }

  return return_value * m_List->GetSortOrder();
}