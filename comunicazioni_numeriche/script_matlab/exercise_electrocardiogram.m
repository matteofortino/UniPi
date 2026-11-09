clear; close all;
% This script computes the Fourier transform
%% Load input signal
dataFile = fullfile(fileparts(mfilename('fullpath')), '100m (1).mat');
if ~isfile(dataFile)
    [selectedFile, selectedPath] = uigetfile('*.mat', 'Select an ECG MAT-file');
    if isequal(selectedFile, 0)
        error('exercise_electrocardiogram:MissingInput', ...
            'Place 100m (1).mat beside this script or select an ECG MAT-file.');
    end
    dataFile = fullfile(selectedPath, selectedFile);
end

loadedData = load(dataFile);
if ~isfield(loadedData, 'val')
    error('exercise_electrocardiogram:MissingVariable', ...
        'The selected MAT-file must contain a variable named val.');
end
val = loadedData.val;
if ~isvector(val)
    warning('exercise_electrocardiogram:MultichannelInput', ...
        'The MAT-file contains multiple channels; plotting the first channel.');
    if size(val, 1) <= size(val, 2)
        val = val(1, :);
    else
        val = val(:, 1);
    end
end
% Sampling frequency in Hz
fs = 360; Ts = 1/fs;
% Center the ECG signal and build a matching time vector.
g = val(:);
g = g - mean(g);
time_samples = (0:numel(g)-1)*Ts;

figure(1); hold on; box; grid on;
plot(time_samples,g,'-k','LineWidth',2)
xlabel('Time in seconds')
ylabel('Time-domain waveform')
%ylim([0 1500])
set(gca,'fontsize',18);


%% Compute Fourier Transform

% Sampling frequency in Hz
deltaf = 0.1;
% Frequency interval in Hz
durF = 100;

% Fourier Transform (Simulated)
[G, frequency_samples] = function_Fourier_Transform(g, time_samples, Ts, deltaf, durF);

% Plot of the absolute valued of Fourier transform 
figure(2); hold on; box; grid on;
plot(frequency_samples, abs(G),'-k','LineWidth',2)
xlabel('Frequency in Hz')
ylabel('Amplitude of Fourier transform')
set(gca,'fontsize',18);
% ylim([-1 1000])
xlim([-durF durF])
