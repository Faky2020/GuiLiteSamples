#include "stdafx.h"
#include "UiBlock.h"

CUiBlock::CUiBlock(int index, int color_bytes)
{
	m_color_bytes = color_bytes;
	m_is_mouse_down = false;
	m_ui_width = m_ui_height = 0;
}

void CUiBlock::renderUI(RECT& rect, HDC hDC)
{
	void* data = NULL;
	if (rect.right == m_block_rect.right && rect.bottom == m_block_rect.bottom &&
		m_ui_width != 0)
	{
		data = getUiOfHelloWindows(NULL, NULL, false);
		goto RENDER;
	}

	m_block_rect = rect;
	data = getUiOfHelloWindows(&m_ui_width, &m_ui_height, true);

	memset(&m_ui_bm_info, 0, sizeof(m_ui_bm_info));
	m_ui_bm_info.bmiHeader.biSize = sizeof(MYBITMAPINFO);
	m_ui_bm_info.bmiHeader.biWidth = m_ui_width;
	m_ui_bm_info.bmiHeader.biHeight = (0 - m_ui_height);//fix upside down. 
	m_ui_bm_info.bmiHeader.biBitCount = m_color_bytes * 8;
	m_ui_bm_info.bmiHeader.biPlanes = 1;
	m_ui_bm_info.bmiHeader.biSizeImage = m_ui_width * m_ui_height * m_color_bytes;

	switch (m_color_bytes)
	{
	case 2:
		m_ui_bm_info.bmiHeader.biCompression = BI_BITFIELDS;
		m_ui_bm_info.biRedMask = 0xF800;
		m_ui_bm_info.biGreenMask = 0x07E0;
		m_ui_bm_info.biBlueMask = 0x001F;
		break;
	case 4:
		m_ui_bm_info.bmiHeader.biCompression = BI_RGB;
		break;
	default:
		break;
	}

	DeleteDC(m_memDC);
	DeleteObject(m_blockBmp);

	m_memDC = CreateCompatibleDC(hDC);
	m_blockBmp = CreateCompatibleBitmap(hDC, (m_block_rect.right - m_block_rect.left), (m_block_rect.bottom - m_block_rect.top));
	SelectObject(m_memDC, m_blockBmp);
	::SetStretchBltMode(m_memDC, STRETCH_HALFTONE);

RENDER:
	if (!data)
	{
		return;
	}
	register int block_width = (m_block_rect.right - m_block_rect.left);
	register int block_height = (m_block_rect.bottom - m_block_rect.top);
	StretchDIBits(m_memDC, 0, 0, block_width, block_height,
		0, 0, m_ui_width, m_ui_height, data, (BITMAPINFO*)&m_ui_bm_info, DIB_RGB_COLORS, SRCCOPY);
	BitBlt(hDC, m_block_rect.left, m_block_rect.top, block_width, block_height, m_memDC, 0, 0, SRCCOPY);
}

void CUiBlock::OnLButtonDown(int x, int y)
{
	m_is_mouse_down = TRUE;
	pointMFC2GuiLite(x, y);
	if (x < 0 || y < 0)
	{
		return;
	}
	sendTouch2HelloWindows(x, y, true);
}

void CUiBlock::OnLButtonUp(int x, int y)
{
	m_is_mouse_down = FALSE;
	pointMFC2GuiLite(x, y);
	if (x < 0 || y < 0)
	{
		return;
	}
	sendTouch2HelloWindows(x, y, false);
}

void CUiBlock::OnMouseMove(int x, int y)
{
	if (m_is_mouse_down == FALSE)
	{
		return;
	}
	OnLButtonDown(x, y);
}

void CUiBlock::pointMFC2GuiLite(int& x, int& y)
{
	if (x > m_block_rect.right || y > m_block_rect.bottom ||
		x < m_block_rect.left || y < m_block_rect.top)
	{
		x = y = 0;
		return;
	}
	x = m_ui_width * (x - m_block_rect.left) / (m_block_rect.right - m_block_rect.left);
	y = m_ui_height * (y - m_block_rect.top) / (m_block_rect.bottom - m_block_rect.top);
}

void CUiBlock::OnKeyUp(unsigned int nChar)
{
}
