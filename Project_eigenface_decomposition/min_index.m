function minIndex=min_index(x)
    minIndex = [];
    if(size(x,2) > 1)
        fprintf('Give a column vector as an input\n')
    else
        min_element = min(x);
        for i = 1:size(x,1)
            if(x(i,1) == min_element)
                minIndex = i;
                break;
            end
        end
    end
end

        
