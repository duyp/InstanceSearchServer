function obj = ISServiceService

obj.endpoint = 'http://192.168.24.59:8080/InstanceSearch/services/ISService';
obj.wsdl = 'http://192.168.24.59:8080/InstanceSearch/services/ISService?wsdl';

obj = class(obj,'ISServiceService');

