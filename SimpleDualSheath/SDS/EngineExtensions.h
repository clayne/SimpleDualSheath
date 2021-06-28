#pragma once

#include "Controller.h"

#include "Events/Dispatcher.h"
#include "Events/CreateWeaponNodesEvent.h"
#include "Events/CreateArmorNodeEvent.h"
#include "Events/OnSetEquipSlot.h"

namespace SDS
{
    class BShkbAnimationGraph;

    class EngineExtensions : 
        ILog
    {
    public:

        EngineExtensions(const EngineExtensions&) = delete;
        EngineExtensions(EngineExtensions&&) = delete;
        EngineExtensions& operator=(const EngineExtensions&) = delete;
        EngineExtensions& operator=(EngineExtensions&&) = delete;

        enum class MemoryValidationFlags : std::uint8_t
        {
            kNone = 0,

            kWeaponLeftAttach = 1ui8 << 0,
            kStaffAttach = 1ui8 << 1,
            kShieldAttach = 1ui8 << 2,
            kDisableShieldHideOnSit = 1ui8 << 3,
            kScabbardAttach = 1ui8 << 4,
            kScabbardDetach = 1ui8 << 5,
            kScabbardGet = 1ui8 << 6,
        };

        static void Initialize(const std::shared_ptr<Controller>& a_controller);
        static MemoryValidationFlags ValidateMemory(const Config& a_config);

        SKMP_FORCEINLINE static auto GetSingleton() {
            return m_Instance.get();
        }
        
        SKMP_FORCEINLINE auto &GetEventDispatchers() {
            return m_dispatchers;
        }

        FN_NAMEPROC("EngineExtensions");

    private:
        EngineExtensions(const std::shared_ptr<Controller>& a_controller);

        void Patch_SCB_Attach();
        void Patch_SCB_Detach();
        void Patch_SCB_Get();
        void Patch_WeaponObjects_Attach();
        void Patch_ShieldObject_Attach();
        void Patch_DisableShieldHideOnSit(const Config& a_config);
        bool Hook_TESObjectWEAP_SetEquipSlot();
        bool Patch_ShieldHandWorkaround();

        static NiNode* GetScbAttachmentNode_Hook(TESObjectREFR* a_actor, TESForm* a_form, NiNode* a_attachmentNode);
        static NiAVObject* GetWeaponShieldSlotNode_Hook(NiNode* a_root, const BSFixedString& a_nodeName, TESObjectREFR* a_ref, Biped* a_biped, std::uint32_t a_bipedSlot, bool a_firstPerson, bool &a_skipCull);
        static NiAVObject* GetWeaponStaffSlotNode_Hook(NiNode* a_root, const BSFixedString& a_nodeName, TESObjectREFR* a_ref, Biped* a_biped, std::uint32_t a_bipedSlot, bool a_firstPerson, bool& a_skipCull);
        static NiAVObject* GetShieldArmorSlotNode_Hook(NiNode* a_root, const BSFixedString& a_nodeName, TESObjectREFR* a_ref, Biped* a_biped, std::uint32_t a_bipedSlot, bool a_firstPerson);
        static NiAVObject* GetScabbardNode_Hook(NiNode* a_object, const BSFixedString& a_nodeName, bool a_left, bool a_firstPerson);
        static void TESObjectWEAP_SetEquipSlot_Hook(BGSEquipType *a_this, BGSEquipSlot* a_slot);

        static bool Unk140609D50_BShkbAnimationGraph_SetGraphVariableInt_Hook(BShkbAnimationGraph* a_graph, const BSFixedString& a_name, std::int32_t a_value, Actor* a_actor);
        static std::uint32_t Unk1406097C0_IAnimationGraphManagerHolder_SetVariableOnGraphsInt_Hook(IAnimationGraphManagerHolder* a_holder, const BSFixedString& a_name, std::int32_t a_value, Actor* a_actor);
        static std::uint32_t Unk140634D20_IAnimationGraphManagerHolder_SetVariableOnGraphsInt_Hook(IAnimationGraphManagerHolder* a_holder, const BSFixedString& a_name, std::int32_t a_value, Actor* a_actor);

        typedef bool(*BShkbAnimationGraph_SetGraphVariableInt_t)(BShkbAnimationGraph* a_graph, const BSFixedString& a_name, std::int32_t a_value);
        typedef std::uint32_t(*IAnimationGraphManagerHolder_SetVariableOnGraphsInt_t)(IAnimationGraphManagerHolder* a_holder, const BSFixedString& a_name, std::int32_t a_value);

        static bool ShouldBlockShieldHide(Actor *a_actor);
        SKMP_NOINLINE const BSFixedString* GetWeaponNodeName(TESObjectREFR* a_ref, Biped* a_biped, std::uint32_t a_bipedSlot, bool a_firstPerson, bool a_left);

        decltype(&TESObjectWEAP_SetEquipSlot_Hook) m_TESObjectWEAP_SetEquipSlot_o;
        BShkbAnimationGraph_SetGraphVariableInt_t m_BShkbAnimationGraph_SetGraphVariableInt_o;
        IAnimationGraphManagerHolder_SetVariableOnGraphsInt_t m_unk1406097C0_IAnimationGraphManagerHolder_SetVariableOnGraphsInt_o;
        IAnimationGraphManagerHolder_SetVariableOnGraphsInt_t m_unk140634D20_IAnimationGraphManagerHolder_SetVariableOnGraphsInt_o;

        typedef NiAVObject* (*fGetNodeByName_t)(NiNode* a_root, const BSFixedString& a_name, bool a_unk);
        typedef NiAVObject* (*fUnk1401CDB30_t)(NiNode*);

        struct
        {
            Events::EventDispatcher<Events::OnSetEquipSlot> m_setEquipSlot;
        } m_dispatchers;

        std::shared_ptr<Controller> m_controller;

        inline static auto m_scbAttach_a = IAL::Address<std::uintptr_t>(15569, 0x3A3);
        inline static auto m_scbGet_a = IAL::Address<std::uintptr_t>(15569, 0x383);
        inline static auto m_getShieldWeaponSlotNode_a = IAL::Address<std::uintptr_t>(15569, 0x1D1);
        inline static auto m_getStaffSlotNode_a = IAL::Address<std::uintptr_t>(15569, 0x223);
        inline static auto m_getShieldArmorSlotNode_a = IAL::Address<std::uintptr_t>(15569, 0x260);
        inline static auto m_scbDetach_a = IAL::Address<std::uintptr_t>(15496, 0x1A3);
        inline static auto m_hideShield_a = IAL::Address<std::uintptr_t>(36580, 0x6); // does other stuff but we don't care here
        inline static auto m_vtbl_TESObjectWEAP = IAL::Address<std::uintptr_t>(234396);
        inline static auto m_fGetNodeByName = IAL::Address<fGetNodeByName_t>(74481);
        inline static auto m_fUnk1401CDB30 = IAL::Address<fUnk1401CDB30_t>(15571);

        inline static auto m_unk140609D50_BShkbAnimationGraph_SetGraphVariableInt_a = IAL::Address<std::uintptr_t>(36957, 0x1DA);               // load (iLeftHandType), rbp - 38 = Actor (BShkbAnimationGraph::SetGraphVariableInt)
        inline static auto m_unk1406097C0_IAnimationGraphManagerHolder_SetVariableOnGraphsInt_a = IAL::Address<std::uintptr_t>(36949, 0x48);    // equip (iLeftHandType), rsi = Actor (IAnimationGraphManagerHolder::SetVariableOnGraphsInt)
        inline static auto m_unk140634D20_IAnimationGraphManagerHolder_SetVariableOnGraphsInt_a = IAL::Address<std::uintptr_t>(37866, 0x17B);   // draw (iLeftHandEquipped), r15 - B8 = Actor (IAnimationGraphManagerHolder::SetVariableOnGraphsInt)

        static std::unique_ptr<EngineExtensions> m_Instance;
    };

    DEFINE_ENUM_CLASS_BITWISE(EngineExtensions::MemoryValidationFlags);
}