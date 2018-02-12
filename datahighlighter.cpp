/*
 * Copyright (c) 2016 Meinhard Ritscher <cyc1ingsir@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * For more information on the GPL, please go to:
 * http://www.gnu.org/copyleft/gpl.html
 */

#include "datahighlighter.h"

DataHighlighter::DataHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
    , m_darkTheme(false)
{
    m_pattern_time = new QRegExp("\\d{2,2}:\\d{2,2}:\\d{2,2}:\\d{3,3} ");
    m_pattern_bytes = new QRegExp("^\\d{8} ");
    m_pattern_ctrl = new QRegExp("[\\x240A\\x240D\\x21E5]");
    m_pattern_hex = new QRegExp("<0x[\\da-f]{2}>");
    DataHighlighter::setupTextFormats();
}

void DataHighlighter::setSearchString(const QString &search)
{
    m_searchString = search;
    rehighlight();
}

void DataHighlighter::setCharFormat(QTextCharFormat *format, DataHighlighter::Formats type)
{
    switch (type) {
    case Formats::HEX:
        // m_format_hex = format;
        break;
    default:
        break;
    }
}

void DataHighlighter::setDarkTheme(bool darkTheme)
{
    m_darkTheme = darkTheme;
    DataHighlighter::setupTextFormats();
}

void DataHighlighter::setupTextFormats()
{
    QColor format_colors[6];
    if (m_darkTheme) {
        /* m_format_time */
        format_colors[0] = Qt::darkGreen;
        /* m_format_bytes. #3daee9 from dark theme */
        format_colors[1] = QColor(61, 174, 233);
        /* m_format_ctrl */
        format_colors[2] = QColor(61, 174, 233);
        /* m_format_hex */
        format_colors[3] = QColor(61, 174, 233);
        /* m_format_search background */
        format_colors[4] = QColor(61, 174, 233);
        /* m_format_search foreground. #31363b from dark theme */
        format_colors[5] = QColor(49, 54, 59);
    } else {
        /* m_format_time */
        format_colors[0] = Qt::darkGreen;
        /* m_format_bytes */
        format_colors[1] = QColor(120, 180, 200);
        /* m_format_ctrl */
        format_colors[2] = Qt::darkRed;
        /* m_format_hex */
        format_colors[3] = Qt::darkMagenta;
        /* m_format_search background */
        format_colors[4] = QColor(230, 230, 180);
        /* m_format_search foreground */
        format_colors[5] = QColor(50, 50, 180);
    }
    m_format_time.setForeground(format_colors[0]);
    m_format_bytes.setForeground(format_colors[1]);
    QFont font;
    font.setFamily(font.defaultFamily());
    font.setPointSize(10);
    m_format_bytes.setFont(font);
    m_format_ctrl.setForeground(format_colors[2]);
    m_format_ctrl.setFontWeight(QFont::Bold);
    font = QFont("Monospace");
    font.setStyleHint(QFont::Courier);
    font.setPointSize(10);
    m_format_hex.setFont(font);
    m_format_hex.setForeground(format_colors[3]);
    m_format_search.setBackground(format_colors[4]);
    m_format_search.setForeground(format_colors[5]);
}

void DataHighlighter::highlightBlock(const QString &text)
{
    if (text.isEmpty())
        return;
    int index = m_pattern_time->indexIn(text);
    if (index >= 0)
        setFormat(index, m_pattern_time->matchedLength(), m_format_time);

    index = m_pattern_bytes->indexIn(text);
    if (index >= 0)
        setFormat(index, m_pattern_bytes->matchedLength(), m_format_bytes);

    index = m_pattern_ctrl->indexIn(text, 0);
    while (index >= 0) {
        setFormat(index, 1, m_format_ctrl);
        index = m_pattern_ctrl->indexIn(text, index + 1);
    }

    index = m_pattern_hex->indexIn(text, 0);
    int l = 0;
    while (index >= 0) {
        l = m_pattern_hex->matchedLength();
        setFormat(index, l, m_format_hex);
        index = m_pattern_hex->indexIn(text, index + l);
    }

    if (m_searchString.isEmpty())
        return;

    const int length = m_searchString.length();
    index = text.indexOf(m_searchString, 0, Qt::CaseInsensitive);
    while (index >= 0) {
        setFormat(index, length, m_format_search);
        index = text.indexOf(m_searchString, index + length, Qt::CaseInsensitive);
    }
}
