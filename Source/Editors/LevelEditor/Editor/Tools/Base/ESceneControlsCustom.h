#pragma once

// refs
class ESceneToolBase;

struct SBeforeAppendCallbackParams{
	LPVOID 		data;
    xr_string 	name_prefix;
    SBeforeAppendCallbackParams()
    {
    	data 		= 0;
        name_prefix	= "";
    }
};
typedef  fastdelegate::FastDelegate1 < SBeforeAppendCallbackParams*, bool> TBeforeAppendCallback;
typedef  fastdelegate::FastDelegate2 < TShiftState , CCustomObject* , bool> TAfterAppendCallback;

class TUI_CustomControl{
protected:
    friend class ESceneToolBase;
    int sub_target;
    int action;
// select
    bool bBoxSelection;
    bool SelectStart	(TShiftState _Shift);
    void SelectProcess	(TShiftState _Shift);
    bool SelectEnd		(TShiftState _Shift);
// add
    bool AddStart		(TShiftState _Shift);
    void AddProcess		(TShiftState _Shift);
    bool AddEnd			(TShiftState _Shift);
    // rotate
	Fvector m_GizmoXVector;
	Fvector m_GizmoYVector;
	Fvector m_GizmoReminder;
	Fvector 			m_RotateVector;
    float 				m_fRotateSnapAngle;

	virtual void MoveStart();
	virtual void ScaleStart();
	virtual void RotateStart();
	virtual void MoveProcess(Fvector Delta, Fvector Vector);
	virtual void ScaleProcess(Fvector Delta, Fvector Vector);
	virtual void RotateProcess(float Delta);

protected:
	bool				CheckSnapList 	(TShiftState Shift);

    CCustomObject*		DefaultAddObject(TShiftState Shift, TBeforeAppendCallback before=0, TAfterAppendCallback after=0);
    bool				DefaultMovingProcess(TShiftState Shift, Fvector& amount);
public:
    ESceneToolBase*	parent_tool;
public:
    				TUI_CustomControl	(int st, int act, ESceneToolBase* parent);
    virtual 		~TUI_CustomControl	(){;}
	virtual bool 	Start  				(TShiftState _Shift);
	virtual bool 	End    				(TShiftState _Shift);
	virtual void 	Move   				(TShiftState _Shift);
	virtual bool 	HiddenMode			();
    virtual bool 	KeyDown				(WORD Key, TShiftState Shift){return false;}
    virtual bool 	KeyUp  				(WORD Key, TShiftState Shift){return false;}
    virtual bool 	KeyPress			(WORD Key, TShiftState Shift){return false;}
    virtual void 	OnEnter				(){;}
    virtual void 	OnExit				(){;}

    int 			Action				(){return action;}

	virtual bool IsSupportMove() { return action == etaSelect; }
	virtual bool IsSupportRotate() { return action == etaSelect; }
	virtual bool IsSupportScale() { return action == etaSelect; }
};