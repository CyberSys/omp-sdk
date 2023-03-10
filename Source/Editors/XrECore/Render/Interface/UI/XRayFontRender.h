#pragma once
class XRayFontRender:public IFontRender
{
public:
	XRayFontRender();
	~XRayFontRender();
	virtual void Initialize(LPCSTR cShader, LPCSTR cTexture) ;
	virtual void OnRender(CGameFont &owner) ;
	void Flush();
private:
	ref_shader				pShader;
	ref_geom				pGeom;
private:
};
