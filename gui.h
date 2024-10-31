#pragma once

namespace gui
{
	void RenderWindow();

	namespace OnReset
	{
		void Before();
		void After();
	}

	void LoadStyle();

	void OnEndScene();
}