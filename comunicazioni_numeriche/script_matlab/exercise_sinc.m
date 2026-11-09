clear; close all;
% This script computes the Fourier transform of a rectangular function

%% Generate a rectangular function of duration T

% Amplitude
A = 1;
% Duration in seconds
T = 0.25;
% Sampling frequency in Hz
fs = 1e4;
% Sampling time in seconds
Ts = 1/fs;
% Time duration
durT = 10;

%% Generate time-domain waveform
% Time samples in seconds
time_samples = -durT:Ts:durT;
% Prepare to save time-domain samples
g = sinc_normalized(time_samples/T);

figure(1); hold on; box; grid on;
plot(time_samples/T,g,'-k','LineWidth',3)
%stem(time_samples,g,'-r','LineWidth',2)
xlabel('Normalized Time t/T')
ylabel('Time-domain waveform')
set(gca,'fontsize',18);
ylim([-0.5 1.5])
xlim([-10 10])

%% Compute Fourier Transform

% Sampling frequency in Hz
% This target resolution keeps the zero-padded FFT manageable.
deltaf = 0.05;

[G_FFT, frequency_samples_FFT] = function_FFT(g, Ts, deltaf);

% Plot of the absolute valued of Fourier transform 
figure(2); hold on; box; grid on;
plot(frequency_samples_FFT, abs(G_FFT)/T,'-r','LineWidth',2)
xlabel('Frequency in Hz')
ylabel('Amplitude Spectrum')
set(gca,'fontsize',18);
ylim([-0.5 1.5])
xlim([-5 5])

% % Plot of the absolute valued of Fourier transform 
% figure(4); hold on; box; grid on;
% plot(frequency_samples, abs(angle(G))/pi,'-k','LineWidth',3)
% % plot(frequency_samples, angle(G_theo),'-r','LineWidth',2)
% %legend('Simulated','Exact')
% xlabel('Frequency in Hz')
% ylabel('Phase of Fourier transform (radians)')
% set(gca,'fontsize',18);
% ylim([-2 2])
% xlim([-durF durF])
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
