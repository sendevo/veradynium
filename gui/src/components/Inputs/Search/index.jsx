import{ useState } from 'react';
import { useTranslation } from 'react-i18next';
import { 
    TextField, 
    InputAdornment, 
    Button,
    Box 
} from '@mui/material';
import { FaSearch, FaTrash } from 'react-icons/fa';
import classes from '../style.module.css';

const SearchInput = ({submit, onChange}) => {

    const [value, setValue] = useState("");
    const [submitted, setSubmitted] = useState(false);

    const {t} = useTranslation('search');

    const handleValueChange = e => {
        setValue(e.target.value);
        if(typeof(onChange) === "function")
            onChange(e.target.value);
    };

    const handleSubmitOrClear = () => {
        setValue(prevValue => {
            const newValue = submitted ? "" : prevValue;
            if(newValue !== "" || submitted)
                setSubmitted(!submitted);
            if(typeof(submit) === "function"){
                submit(newValue);
            }
            return newValue;
        });
    };

    return (
        <TextField
            variant="outlined"
            size="small" 
            className={classes.Input}
            disabled={submitted}
            type="text"
            label={t('text')}
            value={value}
            onChange={handleValueChange}                
            InputProps={{
                endAdornment:<InputAdornment position="end">
                    <Button 
                        variant="text" 
                        sx={{minWidth: 0, m:0}}
                        onClick={handleSubmitOrClear}>
                        {submitted ? <FaTrash/> : <FaSearch/>}
                    </Button>
                </InputAdornment>
            }}/>  
    );
};

export default SearchInput;