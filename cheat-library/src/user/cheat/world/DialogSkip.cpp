#include "pch-il2cpp.h"
#include "DialogSkip.h"

#include <helpers.h>

namespace cheat::feature 
{
    static void InLevelCutScenePageContext_UpdateView_Hook(app::InLevelCutScenePageContext* __this, MethodInfo* method);

    DialogSkip::DialogSkip() : Feature(),
        NFF(m_Enabled, "Auto talk", "m_AutoTalkEnabled", "World", false)
    {
        HookManager::install(app::InLevelCutScenePageContext_UpdateView, InLevelCutScenePageContext_UpdateView_Hook);
    }

    const FeatureGUIInfo& DialogSkip::GetGUIInfo() const
    {
        static const FeatureGUIInfo info{ "", "World", false };
        return info;
    }

    void DialogSkip::DrawMain()
    {
        ConfigWidget(m_Enabled, "Automatic continue dialog. Don't skip talk momently.");
    }

    bool DialogSkip::NeedStatusDraw() const
{
        return m_Enabled;
    }

    void DialogSkip::DrawStatus() 
    {
        ImGui::Text("Auto talk");
    }

    DialogSkip& DialogSkip::GetInstance()
    {
        static DialogSkip instance;
        return instance;
    }

	// Raised when dialog view updating
    // We call free click, if auto talk enabled, that means we just emulate user click
    // When appear dialog choose we create notify with dialog select first item.
    void DialogSkip::OnCutScenePageUpdate(app::InLevelCutScenePageContext* context) 
    {
		if (!m_Enabled)
			return;

		auto talkDialog = context->fields._talkDialog;
		if (talkDialog == nullptr)
			return;

		if (talkDialog->fields._inSelect)
		{
			int32_t value = 0;
			auto object = il2cpp_value_box((Il2CppClass*)*app::Int32__TypeInfo, &value);
			auto notify = app::Notify_CreateNotify_1(nullptr, app::AJAPIFPNFKP__Enum::DialogSelectItemNotify, (app::Object*)object, nullptr);
			app::TalkDialogContext_OnDialogSelectItem(talkDialog, &notify, nullptr);
		}
		else
			app::InLevelCutScenePageContext_OnFreeClick(context, nullptr);
    }

	static void InLevelCutScenePageContext_UpdateView_Hook(app::InLevelCutScenePageContext* __this, MethodInfo* method)
	{
		callOrigin(InLevelCutScenePageContext_UpdateView_Hook, __this, method);

        DialogSkip& dialogSkip = DialogSkip::GetInstance();
        dialogSkip.OnCutScenePageUpdate(__this);
	}
}

