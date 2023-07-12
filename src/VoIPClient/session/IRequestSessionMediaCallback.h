#pragma once

class IRequestSessionMediaCallback {
public:
	virtual void requestVideoQualityChange(int quality) = 0;
};