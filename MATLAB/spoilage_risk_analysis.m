% This MATLAB script is designed to run inside the ThingSpeak
% MATLAB Analysis environment for cloud-based analytics.
% It computes SET, HET, VOC trend, and Spoilage Risk Score (SRS)
% from IoT sensor data uploaded by an ESP32.

readChannelID = 3051991;
readAPIKey  = 'OXJLZOYKI3TGA083'; 
writeAPIKey = 'OP93579NP6JJIYEC'; 

DSTemp = thingSpeakRead(readChannelID, 'Fields', 1, 'NumPoints', 10, 'ReadKey', readAPIKey);
SHTtemp = thingSpeakRead(readChannelID, 'Fields', 2, 'NumPoints', 10, 'ReadKey', readAPIKey);
SHTH = thingSpeakRead(readChannelID, 'Fields', 3, 'NumPoints', 10, 'ReadKey', readAPIKey);
VOC = thingSpeakRead(readChannelID, 'Fields', 4, 'NumPoints', 10, 'ReadKey', readAPIKey);
ProductType = thingSpeakRead(readChannelID, 'Fields', 5, 'NumPoints', 1, 'ReadKey', readAPIKey);
Stage = thingSpeakRead(readChannelID, 'Fields', 6, 'NumPoints', 1, 'ReadKey', readAPIKey);

switch round(ProductType)
    case 1 
        T_safe = 8; H_min = 65; H_max = 85; VOC_base = 50;
    case 2   
        T_safe = 5; H_min = 70; H_max = 90; VOC_base = 40;
    case 3  
        T_safe = 12; H_min = 60; H_max = 80; VOC_base = 60;
    otherwise
        T_safe = 10; H_min = 60; H_max = 85; VOC_base = 50;
end

T_core = mean(DSTemp, 'omitnan');
T_ambient = mean(SHTtemp, 'omitnan');
H = mean(SHTH, 'omitnan');
V = mean(VOC, 'omitnan');

if T_core > T_safe
    SET = T_core - T_safe; 
else
    SET = 0;
end

if H < H_min
    HET = H_min - H;
elseif H > H_max
    HET = H - H_max;
else
    HET = 0;
end

VOC_trend = mean(diff(VOC), 'omitnan');

SRS = 100 * (0.5*(SET/10) + 0.3*(abs(HET)/25) + 0.2*abs(VOC_trend)/5);
SRS = min(SRS, 100);

disp("------------- COLD CHAIN ANALYTICS -------------");
disp(['Product Type: ', num2str(ProductType)]);
disp(['Stage: ', num2str(Stage)]);
disp(['Core Temp (DS18B20): ', num2str(T_core), ' °C']);
disp(['Ambient Temp (SHT): ', num2str(T_ambient), ' °C']);
disp(['Humidity: ', num2str(H), ' %']);
disp(['VOC Proxy: ', num2str(V), ' ppm']);
disp(['SET: ', num2str(SET)]);
disp(['HET: ', num2str(HET)]);
disp(['VOC Trend: ', num2str(VOC_trend)]);
disp(['Spoilage Risk Score (SRS): ', num2str(SRS)]);
disp("-----------------------------------------------");

thingSpeakWrite(readChannelID, 'Fields', [7, 8], ...
    'Values', [SRS, SET], 'WriteKey', writeAPIKey);
