#include "App.h"
#include "GLFW/glfw3.h"
#include "Test.h"
#include "imgui_internal.h"
#include "imgui_te_context.h"
#include "imgui_te_ui.h"
#include "imgui_te_utils.h"

static FuncDoodle::Application* GetApp() {
	FuncDoodle::Application* app =
		static_cast<FuncDoodle::Application*>(ImGui::GetIO().UserData);
	if (!app) {
		app = FuncDoodle::Application::Get();
	}
	return app;
}

static void RegisterTests_FuncDoodle(ImGuiTestEngine* e) {
	ImGuiTest* t = NULL;

	t = IM_REGISTER_TEST(e, "funcdoodle", "menu_open_close");
	t->GuiFunc = [](ImGuiTestContext*) {};
	t->TestFunc = [](ImGuiTestContext* ctx) {
		FuncDoodle::Application* app = GetApp();
		app->Popups()->Open("new");
		ImGui::OpenPopup("NewProj");
		ctx->Yield();

		IM_CHECK(ImGui::IsPopupOpen("NewProj"));
		app->Popups()->Close("new");
		ctx->Yield();

		IM_CHECK(!ImGui::IsPopupOpen("NewProj"));
	};

	t = IM_REGISTER_TEST(e, "funcdoodle", "menu_show_keybinds");
	t->GuiFunc = [](ImGuiTestContext*) {};
	t->TestFunc = [](ImGuiTestContext* ctx) {
		FuncDoodle::Application* app = GetApp();
		app->Popups()->Open("keybinds");
		ImGui::OpenPopup("Keybinds");
		ctx->Yield();

		IM_CHECK(ImGui::IsPopupOpen("Keybinds"));
		app->Popups()->Close("keybinds");
		ctx->Yield();
	};

	t = IM_REGISTER_TEST(e, "funcdoodle", "menu_open_preferences");
	t->GuiFunc = [](ImGuiTestContext*) {};
	t->TestFunc = [](ImGuiTestContext* ctx) {
		FuncDoodle::Application* app = GetApp();
		app->Popups()->Open("pref");
		ImGui::OpenPopup("EditPrefs");
		ctx->Yield();

		IM_CHECK(ImGui::IsPopupOpen("EditPrefs"));
		app->Popups()->Close("pref");
		ctx->Yield();
	};

	t = IM_REGISTER_TEST(e, "funcdoodle", "keybinds_table_headers");
	t->GuiFunc = [](ImGuiTestContext*) {};
	t->TestFunc = [](ImGuiTestContext* ctx) {
		FuncDoodle::Application* app = GetApp();
		app->Popups()->Open("keybinds");
		ImGui::OpenPopup("Keybinds");
		ctx->Yield();

		IM_CHECK(ImGui::IsPopupOpen("Keybinds"));

		app->Popups()->Close("keybinds");
		ctx->Yield();
	};

	t = IM_REGISTER_TEST(e, "funcdoodle", "new_project_dialog_inputs");
	t->GuiFunc = [](ImGuiTestContext*) {};
	t->TestFunc = [](ImGuiTestContext* ctx) {
		FuncDoodle::Application* app = GetApp();
		app->Popups()->Open("new");
		ImGui::OpenPopup("NewProj");
		ctx->Yield();

		IM_CHECK(ImGui::IsPopupOpen("NewProj"));

		app->Popups()->Close("new");
		ctx->Yield();
	};

	t = IM_REGISTER_TEST(e, "funcdoodle", "rotate_dialog");
	t->GuiFunc = [](ImGuiTestContext*) {};
	t->TestFunc = [](ImGuiTestContext* ctx) {
		FuncDoodle::Application* app = GetApp();
		app->Popups()->Open("rotate");
		ImGui::OpenPopup("Rotate");
		ctx->Yield();

		IM_CHECK(ImGui::IsPopupOpen("Rotate"));

		app->Popups()->Close("rotate");
		ctx->Yield();
	};

	t = IM_REGISTER_TEST(e, "funcdoodle", "export_dialog");
	t->GuiFunc = [](ImGuiTestContext*) {};
	t->TestFunc = [](ImGuiTestContext* ctx) {
		FuncDoodle::Application* app = GetApp();
		app->Popups()->Open("export");
		ImGui::OpenPopup("Export##export");
		ctx->Yield();

		ctx->SetRef("Export##export");
		IM_CHECK(ImGui::IsPopupOpen("Export##export"));

		app->Popups()->Close("export");
		ctx->Yield();
	};

	t = IM_REGISTER_TEST(e, "funcdoodle", "test_engine_window");
	t->GuiFunc = [](ImGuiTestContext*) {};
	t->TestFunc = [](ImGuiTestContext* ctx) {
		ctx->SetRef("Dear ImGui Test Engine");
		ImGuiWindow* window = ImGui::FindWindowByName("Dear ImGui Test Engine");
		IM_CHECK(window != nullptr);
	};
}

int FuncDoodle_RegisterImTests() {
	if (s_TestEngine == nullptr)
		return 0;
	RegisterTests_FuncDoodle(s_TestEngine);
	return 0;
}
