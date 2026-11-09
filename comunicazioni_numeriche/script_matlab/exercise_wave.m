clc; close all; clear;


%% Signal 1
[x,Fs] = audioread('StartMeUp_44100Hz_30s.wav');
% listen to the signal 
%sound(x,Fs);
% compute average of left and right signals
x=((x(:,1)+x(:,2))/2);
% plot the time-domain signal
[x, iSampling ] = function_plot(x, Fs);
% plot Fourier Transform
X = function_fft(x, Fs); hold on;
% Reproduce
%sound(x,Fs)

% % Sampling factor
% c = 10;
% % Sampling time
% Fc = Fs/c;
% % Subsampling the wave
% x_sub = x(1:c:end);
% % plot Fourier Transform
% X_sub = function_fft(x_sub, Fc);
% % Reproduce
% sound(x_sub, Fc )

% %% Signal 2
% % frequenza oscillazione seno
% f0 = 19.5e3;
% % genero disturbo
% y = 0.15*sin(2*pi*f0*iSampling);
% % plot the time-domain signal
% [y, iSampling ] = function_plot(y, Fs);
% % plot Fourier Transform
% Y = function_fft(y, Fs);
% % listen to the signal 
% sound(y,Fs);

% % Signal 3
%genero rumore
z = 0.01*randn(length(iSampling),1);
%plot the time-domain signal
[z, iSampling ] = function_plot(z, Fs);
%plot Fourier Transform
Z = function_fft(z, Fs);
%listen to the signal 
sound(z,Fs);



function [x, iSampling ] = function_plot(x, samplingFrequency)
numSamples = length(x);
% signal duration
duration = numSamples/samplingFrequency;
% sampling intervals
iSampling = 0:1/samplingFrequency:duration-1/samplingFrequency;
% plot the time-domain signal
figure; hold on; plot(iSampling, x, LineWidth=2);
xlabel('time in seconds')
set(gca,'fontsize',18);

end

function X = function_fft(x, samplingFrequency)
numSamples = length(x);
% size of FFT
nFFT = 2^nextpow2(numSamples);
% compute FFT
X = fft(x,nFFT)/samplingFrequency;
% istanti di campionamento
iFrequency = (-1/2:1/nFFT:1/2-1/nFFT)*samplingFrequency;
% rappresento graficamente la FFT
figure(2); hold on; grid on; plot(iFrequency/1e3, fftshift(abs(X)), LineWidth=2)
xlabel('Frequency in KHz')
ylabel('Amplitude of Fourier transform')
set(gca,'fontsize',18);
xlim([-15 15]);

% Plot normalized power in dB without requiring pow2db.
powerSpectrum = fftshift(abs(X).^2);
if any(powerSpectrum)
    powerSpectrum_dB = 10*log10(powerSpectrum/max(powerSpectrum));
else
    powerSpectrum_dB = -Inf(size(powerSpectrum));
end
figure(20); hold on; grid on;
plot(iFrequency/1e3, powerSpectrum_dB, LineWidth=2)
xlabel('Frequency in KHz')
ylabel('Normalized power spectrum (dB)')
set(gca,'fontsize',18);
ylim([-100 10]);
xlim([-15 15]);

end
