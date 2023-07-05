interface IServerMediaManager {
public:
	virtual void updateClientVideoQuality(int call_index, int client_index, int level) = 0;
	virtual void registerNotifyTargetVideoQualityCallback(void (*notifyTargetVideoQuality)(int, int)) = 0;
	virtual void startCall(int call_index, auto contact_info, auto operate_info) = 0;
	virtual void endCall(int call_index) = 0;
	virtual void addClient(int call_index, int client_index, auto contact_info) = 0;
	virtual void removeClient(int call_index, int client_index) = 0;
};