import { Button, Box } from '@mui/material';

const inputStyle = {
    display: 'none'
};

const FileInput = ({text, onFileLoaded}) => {

    const handleFileChange = (event) => {
        const selectedFile = event.target.files[0];
        const reader = new FileReader();
        reader.onload = () => { 
            onFileLoaded(reader.result);
        };
        reader.readAsText(selectedFile);
    };

    return (
        <Box>
            <input
                type="file"
                accept=".json"
                onChange={handleFileChange}
                id="file-input"
                style={inputStyle}
            />
            <label htmlFor="file-input">
                <Button 
                    sx={{p:1}}
                    color='primary'
                    variant="contained">
                    {text}
                </Button>
            </label>
        </Box>
    );
};

export default FileInput;