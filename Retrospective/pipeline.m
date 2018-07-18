% ==============================================================================
% title : pipeline.m
% description : Import NihonKoden EEG files in EEGlab and calculate alpha infos
% input: EDF+ files
% output: results summary in CSV file + figure/log for each participant
% author : G. Dumas, Human Genetics & Cognitive Functions, Institut Pasteur
% date : 2018
% version : 2.0
% license : BSD (3-clause)
% ==============================================================================

%% Initialisation
rng(42)
addpath(genpath('PATH_TO_EEGLAB'));
eeglab('nogui');
dataFolder = pwd;

% Parameters
subjectsNum = 135;
inputFolder = 'raw';
outputFolder = 'results';
subjectGroups = {'', 'T'};

mkdir(outputFolder)
fileIDG = fopen([outputFolder,'/Matlab_results.csv'], 'a');

% Header of the CSV file summarizing all the results
fprintf(fileIDG,'Subject,Session,Group,EO,EC,BadChannels,AlphaType,AlphaFreq,AlphaPeak,AlphaChannel,ProcessingDuration\n');

for subjectGroup = 1:2
    %% loop on subjects
    for subjNum = 1:subjectsNum
        display(['Processing subject #', num2str(subjNum)])
        %% list all files
        subjFileBase = ['EEG-', subjectGroups{subjectGroup}, sprintf('%03d', subjNum)];
        files = dir([dataFolder, '/', inputFolder, '/', subjFileBase, '*_0001.edf']);

        if length(files)>1
            display('Multisession subject!')
        end

        if length(files)==0
            display('Missing subject!')
        end

        %% loop on sessions
        for f = 1:length(files)
            tic
            filename = files(f).name;
            display(['Session ', num2str(f)])
            fprintf(fileIDG,'%d,%d,%s,',subjNum,f, subjectGroups{subjectGroup});

            %% Individual log file
            fileID = fopen([outputFolder,'/',filename(1:end-4),'.log'], 'w');
            fprintf(fileID,[filename(1:end-4),'\n']);
            fprintf(fileID,[datestr(clock,'dd-mmm-yyyy HH:MM:SS'),'\n']);

            %% open files
            EEG = pop_biosig( [inputFolder,'/',filename] ,'importevent','off');
            EEG = eeg_checkset( EEG );
            [data,header] = lab_read_edf( [inputFolder,'/',filename] );
            timing = toc;
            fprintf(fileID,'Loading duration: %f seconds\n\n',timing);

            %% channels renamming
            EEG.chanlocs(1).labels = 'Fp1';
            EEG.chanlocs(2).labels = 'Fp2';
            EEG.chanlocs(3).labels = 'F3';
            EEG.chanlocs(4).labels = 'F4';
            EEG.chanlocs(5).labels = 'C3';
            EEG.chanlocs(6).labels = 'C4';
            EEG.chanlocs(7).labels = 'P3';
            EEG.chanlocs(8).labels = 'P4';
            EEG.chanlocs(9).labels = 'O1';
            EEG.chanlocs(10).labels = 'O2';
            EEG.chanlocs(11).labels = 'F7';
            EEG.chanlocs(12).labels = 'F8';
            EEG.chanlocs(13).labels = 'T3';
            EEG.chanlocs(14).labels = 'T4';
            EEG.chanlocs(15).labels = 'T5';
            EEG.chanlocs(16).labels = 'T6';
            EEG.chanlocs(17).labels = 'Fz';
            EEG.chanlocs(18).labels = 'Cz';
            EEG.chanlocs(19).labels = 'Pz';
            EEG = eeg_checkset( EEG );

            %% add channels positions
            EEG=pop_chanedit(EEG, 'lookup','standard-10-5-cap385.elp');
            EEG = eeg_checkset( EEG );

            %% Extract events
            for ch = 1:length(EEG.chanlocs)
                channels{ch} = EEG.chanlocs(ch).labels;
            end

            try
                eventsEDF = [EEG.event.latency];
            catch
                eventsEDF = [EEG.times(1),EEG.times(end)];
            end

            binEO=1;
            binEC=1;
            EyesOpen=[];
            EyesClose=[];

            accu=0;
            for n=1:length(header.events.TYP)
                lab = header.events.TYP{n};

                if strcmp(lab,'Yeux_ouverts')|strcmp(lab,'YEUX_OUVERTS')|strcmp(lab,'YO')
                    display([lab,'+@+',num2str(header.events.POS(n)),'+++',num2str(header.events.POS(n)-accu)])
                    EyesOpen(binEO)=header.events.POS(n);
                    binEO=binEO+1;
                    accu=header.events.POS(n);
                else
                    if (length(lab)>6 & strcmp(lab(1:7),'Yeux_fe'))|strcmp(lab,'Yeux_ferm?s')|strcmp(lab,'Yeux_ferm?s')|strcmp(lab,'YEUX_FERMES')|strcmp(lab,'YF')
                        display([lab,'-@-',num2str(header.events.POS(n)),'---',num2str(header.events.POS(n)-accu)])
                        EyesClose(binEC)=header.events.POS(n);
                        binEC=binEC+1;
                        accu=header.events.POS(n);
                    end
                end
            end

            fprintf(fileIDG,'%d,',length(EyesOpen));
            if length(EyesOpen)>0
                fprintf(fileID,'Eyes open: %d\n',length(EyesOpen));
            else
                fprintf(fileID,'Eyes open: none\n');
            end

            fprintf(fileIDG,'%d,',length(EyesClose));
            if length(EyesClose)>0
                fprintf(fileID,'Eyes close: %d\n',length(EyesOpen));
            else
                fprintf(fileID,'Eyes close: none\n');
            end

            %% Filtering
            EEG = pop_eegfiltnew(EEG, 1, 40, 1650, 0, [], 0);
            EEG = eeg_checkset( EEG );

            %% Detect bad channels and interpolate
            badChannels=zeros(1,19);done=0;
            while done==0
                newSTD=std(EEG.data(badChannels==0,:)');
                newBadChannels=(newSTD>=(mean(newSTD)+2*std(newSTD)));
                if sum(newBadChannels)==0
                    done=1;
                else
                    badChannels(badChannels==0)=newBadChannels;
                end
                badChannels;
            end
            display(channels(badChannels==1))

            EEG = pop_interp(EEG, find(badChannels), 'spherical'); %'spacetime');
            EEG = eeg_checkset( EEG );

            idxBad=find(badChannels);
            fprintf(fileIDG,'%d,',length(idxBad));
            if length(idxBad)>0
                fprintf(fileID,'Bad channels: %d (',length(idxBad));
                for bin = 1:length(idxBad)-1
                    fprintf(fileID,'%s, ',channels{idxBad(bin)});
                end
                if isempty(bin)
                    bin = 0;
                end
                fprintf(fileID,'%s)\n',channels{idxBad(bin+1)});
            else
                fprintf(fileID,'Bad channels: none\n')
            end

            %% Compute spectra
            [spectra,freqs] = spectopo(EEG.data(1:19,:),0,500,'freqrange',[1 40],'plot','off','freqfac',4);
            freqLabels=freqs(freqs<40);
            spectraFull=spectra(:,freqs<40);

            if length(EyesOpen)>0
                spectraEO=zeros(size(spectraFull));
                for binEO = 1:length(EyesOpen)
                    if EyesOpen(binEO)+4000<size(EEG.data,2)
                        if isempty(find(abs(sum(EEG.data(1:19,EyesOpen(binEO)+1000:EyesOpen(binEO)+4000),2))<0.0000001,1))
                            [spectra,freqs] = spectopo(EEG.data(1:19,EyesOpen(binEO)+1000:EyesOpen(binEO)+4000),0,500,'freqrange',[1 40],'plot','off','freqfac',4);
                            spectraEO=spectraEO+spectra(:,freqs<40);
                        else
                            binEO=binEO-1;
                        end
                    else
                        binEO=binEO-1;
                        break
                    end
                end
                spectraEO=spectraEO/binEO;
            else
                spectraEO=zeros(size(spectraFull));
            end

            if length(EyesClose)>0
                spectraEC=zeros(size(spectraFull));
                for binEC = 1:length(EyesClose)
                    if EyesClose(binEC)+4000<size(EEG.data,2)
                        if isempty(find(abs(sum(EEG.data(1:19,EyesClose(binEC)+1000:EyesClose(binEC)+4000),2))<0.0000001,1))
                            [spectra,freqs] = spectopo(EEG.data(1:19,EyesClose(binEC)+1000:EyesClose(binEC)+4000),0,500,'freqrange',[1 40],'plot','off','freqfac',4);
                            spectraEC=spectraEC+spectra(:,freqs<40);
                        else
                            binEO=binEO-1;
                        end
                    else
                        binEC=binEC-1;
                        break
                    end
                end
                spectraEC=spectraEC/binEC;
            else
                spectraEC=zeros(size(spectraFull));       
            end

            %% Plot spectra
            figure(998)
            clf
            imagesc(spectraFull)
            set(gca,'YTick',1:19,'YTickLabel',channels(1:19))
            set(gca,'XTick',1:20:160,'XTickLabel',freqLabels(1:20:160))
            for badChannel = find(badChannels)
                line([1 length(freqLabels)],badChannel*ones(1,2),'color',[1 0 0])
            end
            colorbar
            title('Spectra Full')
            print('-f998','-djpeg',[outputFolder,'/',filename(1:end-4),'_SpectraFull.jpg'])

            figure(999)
            clf
            if length(EyesClose)>0
                imagesc(spectraEC-spectraEO)
                set(gca,'YTick',1:19,'YTickLabel',channels(1:19))
                set(gca,'XTick',1:20:160,'XTickLabel',freqLabels(1:20:160))
                for badChannel = find(badChannels)
                    line([1 length(freqLabels)],badChannel*ones(1,2),'color',[1 0 0])
                end
                colorbar

                % Search alpha peak between 5Hz and 20Hz across posterior sensors
                fSearch=find((freqLabels>5)&(freqLabels<20));
                channelSearch=[7:10 19];
                tmp=spectraEC(channelSearch,fSearch)-spectraEO(channelSearch,fSearch);
                [I,J] = ind2sub(size(tmp),find(tmp==max(tmp(:))));
                alphaChannel=channels{channelSearch(I)};
                alphaPeak=freqLabels(fSearch(J));

                if length(EyesOpen)>0
                    title(sprintf('EC vs EO - Alpha peak at %fHz on channel %s', alphaPeak, alphaChannel))
                    fprintf(fileID,'Alpha peak (EC vs EO) at %fHz on channel %s\n', alphaPeak, alphaChannel);
                    fprintf(fileIDG,'ECvsEO,%f,%f,%s,',alphaPeak, max(tmp(:)), alphaChannel);
                else
                    title(sprintf('EC spectra - Alpha peak at %fHz on channel %s', alphaPeak, alphaChannel))
                    fprintf(fileID,'Alpha peak (EC) at %fHz on channel %s\n', alphaPeak, alphaChannel);
                    fprintf(fileIDG,'EC,%f,%f,%s,',alphaPeak, max(tmp(:)), alphaChannel);
                end
                print('-f999','-djpeg',[outputFolder,'/',filename(1:end-4),'_EC-vs-EO.jpg'])
            else
                fprintf(fileID,'No alpha peak detected...\n');
                fprintf(fileIDG,'None,NaN,NaN,NaN,');
            end

            timing2 = toc;

            %% Clean Workspace
            fprintf(fileID,'\n\nAnalysis duration: %f seconds\n',timing+timing2);
            fprintf(fileIDG,'%f\n',timing+timing2);
            save([outputFolder,'/',filename(1:end-4)], 'EyesClose', 'EyesOpen', 'badChannels', 'spectraEC', 'spectraEO', 'spectraFull', 'timing') 
            fclose(fileID)
            STUDY = []; CURRENTSTUDY = 0; ALLEEG = []; EEG=[]; CURRENTSET=[];
            clc;close all;
        end
    end
end
fclose(fileIDG);